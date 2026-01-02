#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QSet>
#include <QMap>
#include <QVector>

// Struktura logowania ruchu
struct MoveVoteLog {
    QString playerNick;
    QString teamColor;  // "white" or "black"
    QString move;       // m-e8-c7
};

class DummyServer : public QTcpServer {
    Q_OBJECT
public:
    DummyServer(QObject *parent = nullptr);
private slots:
    void onNewConnection();
    void onReadyRead(QTcpSocket *client);
    void broadcastPlayers();
    void startCountdown();
    void sendPlay();
private:
    QSet<QTcpSocket*> clients;
    QSet<QTcpSocket*> readyClients;
    QTimer countdownTimer;
    
    // Move voting
    QMap<QTcpSocket*, QString> clientNick;          // socket -> nick
    QMap<QTcpSocket*, QString> clientTeamColor;     // socket -> white/black/observer
    QMap<QString, QVector<MoveVoteLog>> moveVotes;  // move -> list of votes
    QVector<MoveVoteLog> allMoveLogs;               // wszystkie logi ruchów
    QMap<QString, QString> playerLastVote;          // nick -> last move (aktualna głosowanie)
    
    QSet<QString> usedNicks;  // Zajęte nicki
    int anonCounter = 0;      // Counter dla anonimowych nicków
    
    void broadcastMoveLogs();
};
