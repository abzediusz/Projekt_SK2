#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QList>

// Struktura dla głosu na ruch
struct MoveVote {
    QString teamColor;   // "white", "black", "observer"
    QString playerNick;
    QString move;        // "m-e5-c4" lub "CANCELLED"
};

// Struktura dla gracza w lobby
struct PlayerInfo {
    QString nick;
    QString team;  // "white", "black", "observer"
    bool ready = false;
};

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    
    // Połączenie
    bool connectToServer(const QString &host, int port);
    
    // Logowanie
    void setNick(const QString &nick);
    void setRole(int roleIndex);  // 0=observer, 1=white, 2=black
    void setReady(bool ready);
    void joinGame();
    
    // Gra
    void sendMoveVote(const QString &move);  // "m-e5-c4"
    void requestMoveLogs();
    
    // Gettery
    QString getNick() const { return currentNick; }
    QString getTeamColor() const { return playerColor; }
    
signals:
    // Logowanie
    void nickSet();
    void nickTaken(const QString &nick);
    void roleAcknowledged();
    void connectionSuccess();
    void connectionError(const QString &error);
    
    // Gra
    void teamColorSet(const QString &color);
    void boardUpdate(const QStringList &white, const QStringList &black);
    void moveLogsReceived(const QList<MoveVote> &logs);
    void playersUpdate(int total, int ready);
    void playerListUpdate(const QList<PlayerInfo> &players);
    void countdownStarted();
    void gameStarting();
    void invalidMoveError();
    void gameEnded(const QString &winner);  // "white" or "black"
    
private slots:
    void onConnected();
    void onDisconnected();
    void onError();
    void onReadyRead();
    
private:
    void processMessage(const QString &msg);
    
    QTcpSocket *socket = nullptr;
    QString currentNick;
    QString playerColor = "white";
    int loginStep = 0;  // 0: waiting NICK OK, 1: waiting ROLE OK, 2: waiting JOIN OK, 3: in game
    bool awaitingMoveResponse = false;  // True when waiting for server response to VOTE
};
