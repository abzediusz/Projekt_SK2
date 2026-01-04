#include "lobbywindow.h"
#include "networkmanager.h"
#include "gameboardwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QTimer>
#include <QFont>

LobbyWindow::LobbyWindow(NetworkManager *net, QWidget *parent)
    : QWidget(parent), network(net), countdownSeconds(0), isReady(false)
{
    setWindowTitle("Warcaby - Lobby");
    setFixedSize(790, 465);
    
    // Initialize countdown timer
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &LobbyWindow::updateCountdown);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(15, 15, 15, 15);
    
    // Title
    QLabel *titleLabel = new QLabel("Lobby - Wybierz rolę");
    QFont f = titleLabel->font();
    f.setPointSize(14);
    f.setBold(true);
    titleLabel->setFont(f);
    layout->addWidget(titleLabel);
    
    // Role selector
    layout->addWidget(new QLabel("Twoja rola:"));
    roleCombo = new QComboBox();
    roleCombo->addItem("Obserwator");
    roleCombo->addItem("Białe");
    roleCombo->addItem("Czarne");
    roleCombo->blockSignals(true);  // Blokuj sygnały podczas inicjalizacji
    roleCombo->setCurrentIndex(0); 
    roleCombo->blockSignals(false);
    layout->addWidget(roleCombo);
    
    // Players info
    playersLabel = new QLabel("Graczy w lobby: 0 (0 gotowych)");
    QFont pf = playersLabel->font();
    pf.setPointSize(12);
    playersLabel->setFont(pf);
    playersLabel->setStyleSheet("color: #2196F3; padding: 10px; background-color: #E3F2FD; border-radius: 5px;");
    layout->addWidget(playersLabel);
    
    // Three columns for teams
    QHBoxLayout *teamsLayout = new QHBoxLayout();
    teamsLayout->setSpacing(0);
    teamsLayout->setContentsMargins(0, 0, 0, 0);
    
    // Black team
    QVBoxLayout *blackColumn = new QVBoxLayout();
    blackColumn->setSpacing(0);
    blackColumn->setContentsMargins(0, 0, 0, 0);
    QPushButton *blackLabel = new QPushButton("Czarne");
    blackLabel->setFixedHeight(35);
    blackLabel->setStyleSheet("font-weight: bold; background-color: #424242; color: white; padding: 8px; border: none; border-right: 2px solid #888888;");
    blackLabel->setCursor(Qt::PointingHandCursor);
    connect(blackLabel, &QPushButton::clicked, this, [this]() {
        roleCombo->setCurrentIndex(2);
    });
    blackColumn->addWidget(blackLabel);
    blackList = new QListWidget();
    blackList->setStyleSheet("border: none; border-right: 2px solid #888888; padding: 3px;");
    blackList->setMaximumHeight(150);
    blackColumn->addWidget(blackList);
    teamsLayout->addLayout(blackColumn, 1);
    
    // Observers
    QVBoxLayout *observerColumn = new QVBoxLayout();
    observerColumn->setSpacing(0);
    observerColumn->setContentsMargins(0, 0, 0, 0);
    QPushButton *observerLabel = new QPushButton("Obserwatorzy");
    observerLabel->setFixedHeight(35);
    observerLabel->setStyleSheet("font-weight: bold; background-color: #757575; color: white; padding: 8px; border: none; border-right: 2px solid #888888;");
    observerLabel->setCursor(Qt::PointingHandCursor);
    connect(observerLabel, &QPushButton::clicked, this, [this]() {
        roleCombo->setCurrentIndex(0);
    });
    observerColumn->addWidget(observerLabel);
    observerList = new QListWidget();
    observerList->setStyleSheet("border: none; border-right: 2px solid #888888; padding: 3px;");
    observerList->setMaximumHeight(150);
    observerColumn->addWidget(observerList);
    teamsLayout->addLayout(observerColumn, 1);
    
    // White team
    QVBoxLayout *whiteColumn = new QVBoxLayout();
    whiteColumn->setSpacing(0);
    whiteColumn->setContentsMargins(0, 0, 0, 0);
    QPushButton *whiteLabel = new QPushButton("Białe");
    whiteLabel->setFixedHeight(35);
    whiteLabel->setStyleSheet("font-weight: bold; background-color: #EEEEEE; color: black; padding: 8px; border: none;");
    whiteLabel->setCursor(Qt::PointingHandCursor);
    connect(whiteLabel, &QPushButton::clicked, this, [this]() {
        roleCombo->setCurrentIndex(1);
    });
    whiteColumn->addWidget(whiteLabel);
    whiteList = new QListWidget();
    whiteList->setStyleSheet("border: none; padding: 3px;");
    whiteList->setMaximumHeight(150);
    whiteColumn->addWidget(whiteList);
    teamsLayout->addLayout(whiteColumn, 1);
    
    layout->addLayout(teamsLayout);
    
    // Countdown label
    countdownLabel = new QLabel("");
    countdownLabel->setStyleSheet("background-color: #e74c3c; color: white; padding: 15px; border-radius: 5px; font-size: 16pt; font-weight: bold;");
    countdownLabel->setAlignment(Qt::AlignCenter);
    countdownLabel->setVisible(false);
    layout->addWidget(countdownLabel);
    
    // Ready button
    readyButton = new QPushButton("Gotowy!");
    readyButton->setFixedHeight(40);
    readyButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    layout->addWidget(readyButton);
    
    layout->addStretch();
    
    connect(roleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &LobbyWindow::onRoleChanged);
    connect(readyButton, &QPushButton::clicked, this, &LobbyWindow::onReadyClicked);
    connect(network, &NetworkManager::playersUpdate, this, &LobbyWindow::onPlayersUpdate);
    connect(network, &NetworkManager::playerListUpdate, this, &LobbyWindow::onPlayerListUpdate);
    connect(network, &NetworkManager::countdownStarted, this, &LobbyWindow::onCountdownStarted);
    connect(network, &NetworkManager::gameStarting, this, &LobbyWindow::onGameStarting);
    connect(network, &NetworkManager::spectatorJoinedDuringGame, this, &LobbyWindow::onSpectatorJoinedDuringGame);

    // Ensure the initial role is sent to the server
    onRoleChanged(roleCombo->currentIndex());
}

void LobbyWindow::onRoleChanged(int index)
{
    if (network) {
        network->setRole(index);
    }
}

void LobbyWindow::onReadyClicked()
{
    isReady = !isReady;
    network->setReady(isReady);
    
    if (isReady) {
        readyButton->setText("Anuluj gotowość");
        readyButton->setStyleSheet("background-color: #e67e22; color: white; font-weight: bold;");
    } else {
        readyButton->setText("Gotowy!");
        readyButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold;");
    }
}

void LobbyWindow::onPlayersUpdate(int total, int ready)
{
    playersLabel->setText(QString("Graczy w lobby: %1 (%2 gotowych)").arg(total).arg(ready));
}

void LobbyWindow::onPlayerListUpdate(const QList<PlayerInfo> &players)
{
    whiteList->clear();
    blackList->clear();
    observerList->clear();
    
    QString myNick = network->getNick();
    bool allReady = !players.isEmpty();
    
    for (const PlayerInfo &player : players) {
        QString displayNick = player.nick;
        if (player.ready) {
            displayNick += " ✓";
        } else {
            allReady = false;
        }
        
        QListWidgetItem *item = new QListWidgetItem();
        
        if (player.nick == myNick) {
            displayNick = "★ " + displayNick + " (Ty)";
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
            item->setBackground(QBrush(QColor("#FFEB3B")));
            item->setForeground(QBrush(QColor("#000000")));
        }
        
        item->setText(displayNick);
        
        if (player.team == "white") {
            whiteList->addItem(item);
        } else if (player.team == "black") {
            blackList->addItem(item);
        } else {
            observerList->addItem(item);
        }
    }
    
    // Cancel countdown if someone is no longer ready
    if (!allReady && countdownTimer->isActive()) {
        countdownTimer->stop();
        countdownLabel->setVisible(false);
    }
}

void LobbyWindow::onCountdownStarted()
{
    countdownSeconds = 5;
    countdownLabel->setText(QString("Gra rozpocznie się za %1s...").arg(countdownSeconds));
    countdownLabel->setVisible(true);
    countdownTimer->start(1000);
}

void LobbyWindow::updateCountdown()
{
    countdownSeconds--;
    if (countdownSeconds > 0) {
        countdownLabel->setText(QString("Gra rozpocznie się za %1s...").arg(countdownSeconds));
    } else {
        countdownTimer->stop();
        countdownLabel->setText("START!");
    }
}

void LobbyWindow::onGameStarting()
{
    // Zabezpieczenie przed otwieraniem wielu okien
    if (network->isGameInProgress()) {
        qDebug() << "Game already in progress, ignoring PLAY signal";
        return;
    }
    
    countdownTimer->stop();
    network->setGameInProgress(true);
    
    // NIE wysyłamy JOIN ponownie - został już wysłany w LoginWindow
    // Otwieramy okno gry bezpośrednio
    GameBoardWindow *game = new GameBoardWindow(network);
    
    // Connect return to lobby signal
    connect(game, &GameBoardWindow::returnToLobby, this, [this, game]() {
        // Create new lobby window
        network->setGameInProgress(false);
        LobbyWindow *lobby = new LobbyWindow(network);
        lobby->show();
        game->deleteLater();
    });
    
    game->show();
    this->hide();  // Hide instead of delete to keep network alive
}

void LobbyWindow::onSpectatorJoinedDuringGame()
{
    qDebug() << "Spectator joined during active game - opening game window";
    
    // Klient dołączył jako obserwator w trakcie gry
    // Otwórz okno gry bez możliwości wyboru roli
    GameBoardWindow *game = new GameBoardWindow(network);
    
    // Connect return to lobby signal
    connect(game, &GameBoardWindow::returnToLobby, this, [this, game]() {
        // Create new lobby window
        network->setGameInProgress(false);
        LobbyWindow *lobby = new LobbyWindow(network);
        lobby->show();
        game->deleteLater();
    });
    
    game->show();
    this->hide();
}
