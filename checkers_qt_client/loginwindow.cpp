#include "loginwindow.h"
#include "networkmanager.h"
#include "lobbywindow.h"
#include "gameboardwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent), network(nullptr), gameInProgress(false)
{
    setWindowTitle("Warcaby - Logowanie");
    setGeometry(100, 100, 400, 350);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // Title
    QLabel *titleLabel = new QLabel("Połącz się do serwera");
    QFont f = titleLabel->font();
    f.setPointSize(14);
    f.setBold(true);
    titleLabel->setFont(f);
    layout->addWidget(titleLabel);
    
    // Host
    hostInput = new QLineEdit("127.0.0.1");
    hostInput->setPlaceholderText("Host");
    layout->addWidget(new QLabel("Host:"));
    layout->addWidget(hostInput);
    
    // Port
    portInput = new QLineEdit("12345");
    portInput->setPlaceholderText("Port");
    layout->addWidget(new QLabel("Port:"));
    layout->addWidget(portInput);
    
    // Nick
    nickInput = new QLineEdit();
    nickInput->setPlaceholderText("Nick (pozostaw puste dla guest)");
    layout->addWidget(new QLabel("Nick:"));
    layout->addWidget(nickInput);
    
    // Status
    statusLabel = new QLabel("");
    layout->addWidget(statusLabel);
    
    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    connectButton = new QPushButton("Połącz");
    connectButton->setMinimumHeight(40);
    QPushButton *quitBtn = new QPushButton("Wyjdź");
    quitBtn->setMinimumHeight(40);
    btnLayout->addWidget(connectButton);
    btnLayout->addWidget(quitBtn);
    layout->addLayout(btnLayout);
    
    layout->addStretch();
    
    connect(connectButton, &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
    connect(quitBtn, &QPushButton::clicked, this, &QWidget::close);
}

void LoginWindow::onConnectClicked()
{
    QString host = hostInput->text();
    QString portStr = portInput->text();
    QString nick = nickInput->text();
    
    if (host.isEmpty() || portStr.isEmpty()) {
        statusLabel->setText("Błąd: Podaj host i port");
        statusLabel->setStyleSheet("color: red;");
        return;
    }
    
    bool ok;
    int port = portStr.toInt(&ok);
    if (!ok || port <= 0) {
        statusLabel->setText("Błąd: Zły port");
        statusLabel->setStyleSheet("color: red;");
        return;
    }
    
    statusLabel->setText("Łączenie...");
    statusLabel->setStyleSheet("color: blue;");
    connectButton->setEnabled(false);
    
    network = new NetworkManager(this);
    
    // Podłącz się do gameStarting PRZED innymi sygnałami
    connect(network, &NetworkManager::gameStarting, this, [this]() {
        gameInProgress = true;
        statusLabel->setText("Gra w trakcie - dołączanie jako obserwator...");
        statusLabel->setStyleSheet("color: orange;");
    });
    
    // Login sequence
    connect(network, &NetworkManager::connectionSuccess, this, [this, nick]() {
        statusLabel->setText("Połączono, wysyłam nick...");
        network->setNick(nick);
    });
    
    connect(network, &NetworkManager::nickSet, this, [this]() {
        // Czekamy chwilę na ewentualne PLAY (jeśli gra w trakcie)
        QTimer::singleShot(200, this, [this]() {
            if (gameInProgress) {
                // Gra już trwa - dołącz jako obserwator
                network->setRole(0);  // observer
                QTimer::singleShot(100, this, [this]() {
                    network->joinGame();
                    GameBoardWindow *game = new GameBoardWindow(network);
                    game->show();
                    this->hide();
                });
            } else {
                // Normalne lobby
                statusLabel->setText("Otwieranie lobby...");
                statusLabel->setStyleSheet("color: green;");
                LobbyWindow *lobby = new LobbyWindow(network);
                lobby->show();
                this->hide();
            }
        });
    });
    
    connect(network, &NetworkManager::nickTaken, this, [this](const QString &nick) {
        statusLabel->setText("Nick \"" + nick + "\" jest już zajęty! Wybierz inny.");
        statusLabel->setStyleSheet("color: red; font-weight: bold;");
        connectButton->setEnabled(true);
        nickInput->setFocus();
        nickInput->selectAll();
        // Disconnect and delete network to allow reconnection
        if (network) {
            network->deleteLater();
            network = nullptr;
        }
    });
    
    connect(network, &NetworkManager::connectionError, this, [this](const QString &err) {
        statusLabel->setText("Błąd: " + err);
        statusLabel->setStyleSheet("color: red;");
        connectButton->setEnabled(true);
    });
    
    network->connectToServer(host, port);
}
