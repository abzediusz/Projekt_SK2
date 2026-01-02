#pragma once
#include <QWidget>

class QComboBox;
class QPushButton;
class QLabel;
class QListWidget;
class QTimer;
class NetworkManager;
struct PlayerInfo;

class LobbyWindow : public QWidget {
    Q_OBJECT
public:
    explicit LobbyWindow(NetworkManager *net, QWidget *parent = nullptr);
    
private slots:
    void onRoleChanged(int index);
    void onReadyClicked();
    void onPlayersUpdate(int total, int ready);
    void onPlayerListUpdate(const QList<PlayerInfo> &players);
    void onCountdownStarted();
    void onGameStarting();
    void updateCountdown();
    
private:
    NetworkManager *network;
    QComboBox *roleCombo;
    QPushButton *readyButton;
    QLabel *playersLabel;
    QLabel *countdownLabel;
    QListWidget *whiteList;
    QListWidget *blackList;
    QListWidget *observerList;
    QTimer *countdownTimer;
    int countdownSeconds;
    bool isReady;
};
