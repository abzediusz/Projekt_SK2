#include "movevotingpanel.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QHeaderView>
#include <QFont>
#include <QMap>
#include <QPushButton>
#include <QDebug>

MoveVotingPanel::MoveVotingPanel(NetworkManager *net, QWidget *parent)
    : QWidget(parent), network(net)
{
    setMaximumWidth(300);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // Team label
    teamLabel = new QLabel("Drużyna: ...");
    teamLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    teamLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(teamLabel);
    
    // Title
    QLabel *titleLabel = new QLabel("GŁOSY NA RUCHY");
    QFont f = titleLabel->font();
    f.setPointSize(11);
    f.setBold(true);
    titleLabel->setFont(f);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // Top 5 moves table
    topMovesTable = new QTableWidget(5, 2);
    topMovesTable->setHorizontalHeaderLabels(QStringList() << "Ruch" << "Głosy");
    topMovesTable->horizontalHeader()->setStretchLastSection(false);
    topMovesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    topMovesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    topMovesTable->setColumnWidth(1, 60);
    topMovesTable->verticalHeader()->setVisible(false);
    topMovesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    topMovesTable->setSelectionMode(QAbstractItemView::NoSelection);
    topMovesTable->setMaximumHeight(150);
    topMovesTable->setStyleSheet("QTableWidget { border: 1px solid #bdc3c7; } QHeaderView::section { background-color: #34495e; color: white; font-weight: bold; padding: 4px; }");
    layout->addWidget(topMovesTable);
    
    // Cancel vote button
    cancelButton = new QPushButton("Anuluj głos");
    cancelButton->setEnabled(false);
    cancelButton->setFixedHeight(32);
    cancelButton->setStyleSheet("QPushButton:enabled { background-color: #e74c3c; color: white; font-weight: bold; padding: 6px; } QPushButton:disabled { background-color: #bdc3c7; color: #7f8c8d; }");
    connect(cancelButton, &QPushButton::clicked, this, &MoveVotingPanel::onCancelVote);
    layout->addWidget(cancelButton);
    
    // Spacing between button and logs
    layout->addSpacing(30);
    
    // Logs
    logList = new QListWidget();
    logList->setStyleSheet("QListWidget { border: 1px solid #bdc3c7; border-radius: 4px; }");
    logList->setMaximumHeight(150);
    layout->addWidget(logList);
    
    layout->addStretch();
    
    // Signals
    if (network) {
        connect(network, &NetworkManager::teamColorSet, this, &MoveVotingPanel::onTeamColorSet);
        connect(network, &NetworkManager::moveLogsReceived, this, &MoveVotingPanel::onMoveLogsReceived);
        
        // Initialize teamColor from NetworkManager
        QString initialColor = network->getTeamColor();
        if (!initialColor.isEmpty()) {
            onTeamColorSet(initialColor);
        }
    }
}

void MoveVotingPanel::onTeamColorSet(const QString &color)
{
    teamColor = color;
    
    QString teamName = (color == "white") ? "Biała" : 
                       (color == "black") ? "Czarna" : "Obserwator";
    teamLabel->setText("Drużyna: " + teamName);
    
    qDebug() << "Team color set:" << color;
}

void MoveVotingPanel::onMoveLogsReceived(const QList<MoveVote> &logs)
{
    qDebug() << "Received" << logs.size() << "move logs";
    
    logList->clear();
    
    // Filter logs by team color (observers see nothing)
    QList<MoveVote> filteredLogs;
    for (const MoveVote &log : logs) {
        if (teamColor == "observer") {
            // Observers see nothing
            continue;
        }
        if (log.teamColor == teamColor) {
            filteredLogs.append(log);
        }
    }
    
    // Display filtered logs
    for (const MoveVote &log : filteredLogs) {
        QString teamSymbol = (log.teamColor == "white") ? "○" : "●";
        QString text = log.move == "CANCELLED" ?
            QString("%1 %2: ANULOWANO").arg(teamSymbol, log.playerNick) :
            QString("%1 %2: %3").arg(teamSymbol, log.playerNick, formatMove(log.move));
        
        QListWidgetItem *item = new QListWidgetItem(text);
        
        // Color by team
        if (log.teamColor == "white") {
            item->setBackground(QColor(255, 255, 255));
            item->setForeground(QColor(0, 0, 0));
        } else {
            item->setBackground(QColor(0, 0, 0));
            item->setForeground(QColor(255, 255, 255));
        }
        
        logList->addItem(item);
    }
    
    // Find latest vote per player (only keep the most recent)
    QMap<QString, QString> latestVoteByPlayer;
    for (const MoveVote &log : filteredLogs) {
        if (log.move != "CANCELLED") {
            latestVoteByPlayer[log.playerNick] = log.move;
        } else {
            latestVoteByPlayer.remove(log.playerNick);
        }
    }
    
    // Count votes
    QMap<QString, int> moveCounts;
    for (auto it = latestVoteByPlayer.begin(); it != latestVoteByPlayer.end(); ++it) {
        moveCounts[it.value()]++;
    }
    
    // Sort moves by count (descending)
    QList<QPair<int, QString>> sortedMoves;
    for (auto it = moveCounts.begin(); it != moveCounts.end(); ++it) {
        sortedMoves.append(qMakePair(it.value(), it.key()));
    }
    std::sort(sortedMoves.begin(), sortedMoves.end(), [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
        return a.first > b.first;
    });
    
    // Fill top 5 table
    for (int i = 0; i < 5; ++i) {
        if (i < sortedMoves.size()) {
            QTableWidgetItem *moveItem = new QTableWidgetItem(formatMove(sortedMoves[i].second));
            QTableWidgetItem *countItem = new QTableWidgetItem(QString::number(sortedMoves[i].first));
            
            moveItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            countItem->setTextAlignment(Qt::AlignCenter);
            
            // Highlight top move
            if (i == 0) {
                QFont boldFont;
                boldFont.setBold(true);
                moveItem->setFont(boldFont);
                countItem->setFont(boldFont);
                moveItem->setBackground(QBrush(QColor("#3498db")));
                moveItem->setForeground(QBrush(QColor("white")));
                countItem->setBackground(QBrush(QColor("#3498db")));
                countItem->setForeground(QBrush(QColor("white")));
            }
            
            topMovesTable->setItem(i, 0, moveItem);
            topMovesTable->setItem(i, 1, countItem);
        } else {
            topMovesTable->setItem(i, 0, new QTableWidgetItem(""));
            topMovesTable->setItem(i, 1, new QTableWidgetItem(""));
        }
    }
}

QString MoveVotingPanel::formatMove(const QString &move) const
{
    if (move.startsWith("m-")) {
        QStringList parts = move.mid(2).split('-');
        if (parts.size() == 2) {
            return QString("%1 → %2").arg(parts[0].toUpper(), parts[1].toUpper());
        }
    }
    return move.toUpper();
}

void MoveVotingPanel::onCancelVote()
{
    if (!currentVote.isEmpty() && network) {
        network->sendMoveVote("CANCELLED");
        QString teamSymbol = (teamColor == "white") ? "○" : "●";
        QString playerNick = network->getNick();
        logList->addItem(QString("%1 %2: Anulowano głos").arg(teamSymbol, playerNick));
        currentVote.clear();
        cancelButton->setEnabled(false);
    }
}

void MoveVotingPanel::onVoteSent(const QString &move)
{
    currentVote = move;
    cancelButton->setEnabled(true);
}
