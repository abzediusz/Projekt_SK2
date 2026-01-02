#include "votingpanel.h"
#include "networkmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

VotingPanel::VotingPanel(NetworkManager *network, QWidget *parent)
    : QWidget(parent), network(network)
{
    setMaximumWidth(250);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Title
    QLabel *titleLabel = new QLabel("GŁOSOWANIE");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Options A, B, C, D
    const char *optionNames[] = {"Opcja A", "Opcja B", "Opcja C", "Opcja D"};
    const int optionValues[] = {1, 2, 3, 4};
    const QColor buttonColors[] = {QColor(52, 152, 219), QColor(46, 204, 113), 
                                   QColor(241, 196, 15), QColor(230, 126, 34)};
    
    for (int i = 0; i < 4; ++i) {
        QGroupBox *optionGroup = new QGroupBox(optionNames[i], this);
        QVBoxLayout *optionLayout = new QVBoxLayout();
        
        // Vote button
        int optionValue = optionValues[i];
        voteButtons[i] = new QPushButton(QString("Głosuj"));
        voteButtons[i]->setMinimumHeight(35);
        QString buttonStyle = QString("background-color: %1; color: white; font-weight: bold; border-radius: 5px;")
            .arg(buttonColors[i].name());
        voteButtons[i]->setStyleSheet(buttonStyle);
        connect(voteButtons[i], &QPushButton::clicked, [this, optionValue]() { onVoteButton(optionValue); });
        optionLayout->addWidget(voteButtons[i]);
        
        // Result label
        resultLabels[i] = new QLabel("Głosów: 0");
        resultLabels[i]->setAlignment(Qt::AlignCenter);
        QFont resultFont = resultLabels[i]->font();
        resultFont.setPointSize(10);
        resultFont.setBold(true);
        resultLabels[i]->setFont(resultFont);
        optionLayout->addWidget(resultLabels[i]);
        
        optionGroup->setLayout(optionLayout);
        mainLayout->addWidget(optionGroup);
    }
    
    // Status label
    statusLabel = new QLabel("Gotowy do głosowania");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    mainLayout->addWidget(statusLabel);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *refreshBtn = new QPushButton("Odśwież");
    refreshBtn->setMinimumHeight(30);
    connect(refreshBtn, &QPushButton::clicked, this, &VotingPanel::onRefreshResults);
    buttonLayout->addWidget(refreshBtn);
    
    QPushButton *clearBtn = new QPushButton("Cofnij");
    clearBtn->setMinimumHeight(30);
    connect(clearBtn, &QPushButton::clicked, this, &VotingPanel::onClearVote);
    buttonLayout->addWidget(clearBtn);
    
    mainLayout->addLayout(buttonLayout);
    
    mainLayout->addStretch();
    
    // Connect network signals
    if (network) {
        connect(network, &NetworkManager::voteResultsReceived, this, &VotingPanel::onVoteResultsReceived);
        connect(network, &NetworkManager::votingEnded, this, &VotingPanel::onVotingEnded);
        connect(network, &NetworkManager::voteConfirmed, this, &VotingPanel::onVoteConfirmed);
    }
}

void VotingPanel::onVoteButton(int option)
{
    if (network) {
        network->sendVote(option);
        currentVote = option;
        statusLabel->setText(QString("Głos na opcję %1").arg(option));
        statusLabel->setStyleSheet("color: green;");
    }
}

void VotingPanel::onRefreshResults()
{
    if (network) {
        network->requestResults();
        statusLabel->setText("Pobieranie wyników...");
        statusLabel->setStyleSheet("color: blue;");
    }
}

void VotingPanel::onClearVote()
{
    if (network) {
        network->sendVote(0);
        currentVote = 0;
        statusLabel->setText("Głos cofnięty");
        statusLabel->setStyleSheet("color: orange;");
    }
}

void VotingPanel::onVoteResultsReceived(int opt1, int opt2, int opt3, int opt4)
{
    updateResultDisplay(opt1, opt2, opt3, opt4);
    statusLabel->setText("Wyniki zaktualizowane");
    statusLabel->setStyleSheet("color: blue;");
}

void VotingPanel::onVotingEnded(int opt1, int opt2, int opt3, int opt4, const QString &winner)
{
    updateResultDisplay(opt1, opt2, opt3, opt4);
    
    // Disable voting buttons
    for (int i = 0; i < 4; ++i) {
        voteButtons[i]->setEnabled(false);
    }
    
    statusLabel->setText("Głosowanie zakończone!\nWygrywający: " + winner);
    statusLabel->setStyleSheet("color: red; font-weight: bold;");
}

void VotingPanel::onVoteConfirmed(int option)
{
    currentVote = option;
    statusLabel->setText(QString("Głos na opcję %1 potwierdzony").arg(option));
    statusLabel->setStyleSheet("color: green;");
}

void VotingPanel::updateResultDisplay(int opt1, int opt2, int opt3, int opt4)
{
    const int options[] = {opt1, opt2, opt3, opt4};
    for (int i = 0; i < 4; ++i) {
        resultLabels[i]->setText(QString("Głosów: %1").arg(options[i]));
    }
}
