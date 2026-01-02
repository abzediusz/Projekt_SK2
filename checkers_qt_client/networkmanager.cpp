#include "networkmanager.h"
#include <QDebug>
#include <QAbstractSocket>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
{
}

NetworkManager::~NetworkManager()
{
    if (socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
}

bool NetworkManager::connectToServer(const QString &host, int port)
{
    if (socket) socket->deleteLater();
    
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &NetworkManager::onError);
    connect(socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    
    socket->connectToHost(host, port);
    return socket->waitForConnected(5000);
}

void NetworkManager::setNick(const QString &nick)
{
    currentNick = nick.isEmpty() ? "guest" : nick;
    QString msg = QString("NICK %1\n").arg(currentNick);
    if (socket) socket->write(msg.toUtf8());
}

void NetworkManager::setRole(int roleIndex)
{
    QString role;
    switch (roleIndex) {
        case 0: role = "observer"; break;
        case 1: role = "white"; break;
        case 2: role = "black"; break;
        default: role = "observer";
    }
    playerColor = role;
    
    QString msg = QString("ROLE %1\n").arg(role);
    if (socket) socket->write(msg.toUtf8());
}

void NetworkManager::joinGame()
{
    if (socket) socket->write("JOIN\n");
}

void NetworkManager::setReady(bool ready)
{
    if (ready) {
        if (socket) socket->write("R\n");
    } else {
        if (socket) socket->write("UNREADY\n");
    }
}

void NetworkManager::sendMoveVote(const QString &move)
{
    QString cmd = QString("VOTE %1\n").arg(move);
    if (socket) socket->write(cmd.toUtf8());
}

void NetworkManager::requestMoveLogs()
{
    if (socket) socket->write("MOVELOGS\n");
}

void NetworkManager::onConnected()
{
    qDebug() << "✓ Connected to server";
    emit connectionSuccess();
}

void NetworkManager::onDisconnected()
{
    qDebug() << "✗ Disconnected";
}

void NetworkManager::onError()
{
    if (socket) {
        emit connectionError(socket->errorString());
    }
}

void NetworkManager::onReadyRead()
{
    if (!socket) return;
    
    while (socket->canReadLine()) {
        QString msg = QString::fromUtf8(socket->readLine()).trimmed();
        if (!msg.isEmpty()) {
            processMessage(msg);
        }
    }
}

void NetworkManager::processMessage(const QString &msg)
{
    qDebug() << "RX:" << msg;
    
    // Login: OK responses
    if (msg == "OK") {
        if (loginStep == 0) {
            qDebug() << "  → NICK OK";
            loginStep = 1;
            emit nickSet();
        } else if (loginStep == 1) {
            qDebug() << "  → ROLE OK";
            loginStep = 2;
            emit roleAcknowledged();
        }
        return;
    }
    
    // Joined game
    if (msg.startsWith("Dołączono")) {
        qDebug() << "  → Joined, team:" << playerColor;
        loginStep = 3;
        emit teamColorSet(playerColor);
        requestMoveLogs();
        return;
    }
    
    // Board: BOARD-e2,d3,Ke8|a7,b6
    if (msg.startsWith("BOARD-")) {
        QString boardState = msg.mid(6).trimmed();
        QStringList pieces = boardState.split('|');
        if (pieces.size() >= 2) {
            emit boardUpdate(pieces[0].split(','), pieces[1].split(','));
        }
        return;
    }
    
    // Logs: MOVELOGS white:nick:m-e5-c4;black:nick:CANCELLED
    if (msg.startsWith("MOVELOGS")) {
        QList<MoveVote> logs;
        QString logStr = msg.mid(9).trimmed();
        
        for (const QString &entry : logStr.split(';')) {
            if (entry.isEmpty()) continue;
            QStringList parts = entry.split(':');
            if (parts.size() == 3) {
                MoveVote vote;
                vote.teamColor = parts[0];
                vote.playerNick = parts[1];
                vote.move = parts[2];
                logs.append(vote);
            }
        }
        
        emit moveLogsReceived(logs);
        return;
    }
    
    // Players: PLAYERS-total-ready
    if (msg.startsWith("PLAYERS-")) {
        QString data = msg.mid(8);
        QStringList parts = data.split('-');
        if (parts.size() == 2) {
            int total = parts[0].toInt();
            int ready = parts[1].toInt();
            emit playersUpdate(total, ready);
        }
        return;
    }
    
    // Player list: PLAYERLIST-nick1:team1:ready1;nick2:team2:ready2
    if (msg.startsWith("PLAYERLIST-")) {
        QString data = msg.mid(11);
        QList<PlayerInfo> players;
        for (const QString &entry : data.split(';', Qt::SkipEmptyParts)) {
            QStringList parts = entry.split(':');
            if (parts.size() >= 2) {
                PlayerInfo info;
                info.nick = parts[0];
                info.team = parts[1];
                info.ready = (parts.size() >= 3 && parts[2] == "1");
                players.append(info);
            }
        }
        emit playerListUpdate(players);
        return;
    }
    
    // Countdown
    if (msg == "COUNTDOWN") {
        qDebug() << "  → Countdown starting!";
        emit countdownStarted();
        return;
    }
    
    // Game starting
    if (msg == "PLAY") {
        qDebug() << "  → Game starting!";
        emit gameStarting();
        return;
    }
    
    // Invalid move
    if (msg == "ERROR" || msg.startsWith("ERROR")) {
        qDebug() << "  → Invalid move!";
        emit invalidMoveError();
        return;
    }
}
