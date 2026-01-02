#include "dummy_server.h"
#include <QCoreApplication>

DummyServer::DummyServer(QObject *parent) : QTcpServer(parent) {
    listen(QHostAddress::Any, 12345);
    connect(this, &QTcpServer::newConnection, this, &DummyServer::onNewConnection);
    QTimer::singleShot(1000, this, &DummyServer::broadcastPlayers);
}

void DummyServer::onNewConnection() {
    auto *client = nextPendingConnection();
    clients.insert(client);
    connect(client, &QTcpSocket::readyRead, this, [=]{ onReadyRead(client); });
    connect(client, &QTcpSocket::disconnected, this, [=]{
        clients.remove(client);
        readyClients.remove(client);
        // Usuń zarezerwowany nick
        QString nick = clientNick.value(client);
        if (!nick.isEmpty()) {
            usedNicks.remove(nick);
        }
        clientNick.remove(client);
        clientTeamColor.remove(client);
        broadcastPlayers();
        client->deleteLater();
    });
}

void DummyServer::onReadyRead(QTcpSocket *client) {
    while (client->canReadLine()) {
        QString msg = QString::fromUtf8(client->readLine()).trimmed();
        
        // Nick setting
        if (msg.startsWith("NICK ")) {
            QString nick = msg.mid(5).trimmed();
            
            // Jeśli pusty, generuj anonimXYZ
            if (nick.isEmpty()) {
                nick = QString("anonim%1").arg(++anonCounter);
            }
            
            // Sprawdź zajętość
            if (usedNicks.contains(nick)) {
                client->write("ERR NICK_ZAJETY\n");
                continue;
            }
            
            // Zapamiętaj nick
            usedNicks.insert(nick);
            clientNick[client] = nick;
            client->write("OK\n");
            continue;
        }
        
        // Role setting
        if (msg.startsWith("ROLE ")) {
            QString role = msg.mid(5).trimmed();
            clientTeamColor[client] = role;
            client->write("OK\n");
            continue;
        }
        
        // Join game
        if (msg == "JOIN") {
            client->write("Dołączono do gry.\n");
            // Wyślij początkową planszę
            client->write("BOARD-e2,d3,Ke8|a7,b6\n");
            broadcastMoveLogs();  // Wyślij initial logs
            continue;
        }
        
        // Move voting
        if (msg.startsWith("VOTE ")) {
            QString nick = clientNick.value(client, "unknown");
            QString teamColor = clientTeamColor.value(client, "observer");
            
            // Obserwator nie może głosować
            if (teamColor == "observer") {
                client->write("ERR OBSERVER_CANNOT_VOTE\n");
                continue;
            }
            
            QString move = msg.mid(5).trimmed();  // m-e8-c7
            if (move.startsWith("m-")) {
                // Zapamiętaj ostatni głos gracza (override poprzedni)
                playerLastVote[nick] = move;
                
                // Dodaj log dla historii
                MoveVoteLog log;
                log.playerNick = nick;
                log.teamColor = teamColor;
                log.move = move;
                allMoveLogs.append(log);
                
                client->write("OK\n");
                broadcastMoveLogs();
                continue;
            }
        }
        
        // Cancel vote
        if (msg == "CANCEL_VOTE") {
            QString nick = clientNick.value(client, "unknown");
            QString teamColor = clientTeamColor.value(client, "observer");
            
            if (teamColor == "observer") {
                client->write("ERR OBSERVER_CANNOT_CANCEL\n");
                continue;
            }
            
            // Usuń ostatni głos gracza
            playerLastVote.remove(nick);
            
            // Dodaj log anulowania
            MoveVoteLog log;
            log.playerNick = nick;
            log.teamColor = teamColor;
            log.move = "CANCELLED";
            allMoveLogs.append(log);
            
            client->write("OK\n");
            broadcastMoveLogs();
            continue;
        }
        
        // Request move logs
        if (msg == "MOVELOGS") {
            QString logStr;
            // Wysyłamy CAŁĄ historię głosów (allMoveLogs)
            for (const MoveVoteLog &log : allMoveLogs) {
                if (!logStr.isEmpty()) logStr += ";";
                logStr += QString("%1:%2:%3")
                    .arg(log.teamColor, log.playerNick, log.move);
            }
            QByteArray response = QString("MOVELOGS %1\n").arg(logStr).toUtf8();
            client->write(response);
            continue;
        }
        
        // Ready (old protocol)
        if (msg == "R") {
            readyClients.insert(client);
            broadcastPlayers();
            if (!clients.isEmpty() && readyClients.size() == clients.size()) {
                startCountdown();
            }
            continue;
        }
        
        // Unready
        if (msg == "UNREADY") {
            readyClients.remove(client);
            // Cancel countdown if it was running
            if (countdownTimer.isActive()) {
                countdownTimer.stop();
            }
            broadcastPlayers();
            continue;
        }
        
        // Old move protocol
        if (msg.startsWith("m-")) {
            static bool toggle = false;
            toggle = !toggle;
            if (toggle) {
                client->write("MOVE-OK-e4-c5\n");
                client->write("BOARD-e2,d3,Ke8|a7,b6\n");
            } else {
                client->write("MOVE-error-e4-c5\n");
            }
        }
    }
}

void DummyServer::broadcastPlayers() {
    int total = clients.size();
    int ready = readyClients.size();
    QByteArray msg = QString("PLAYERS-%1-%2\n").arg(total).arg(ready).toUtf8();
    
    // Wyślij listę graczy z nickami i drużynami
    // Format: PLAYERLIST-nick1:team1:ready1;nick2:team2:ready2
    QString playerList;
    for (auto *c : clients) {
        QString nick = clientNick.value(c, "guest");
        QString team = clientTeamColor.value(c, "observer");
        bool ready = readyClients.contains(c);
        if (!playerList.isEmpty()) playerList += ";";
        playerList += QString("%1:%2:%3").arg(nick, team, ready ? "1" : "0");
    }
    QByteArray listMsg = QString("PLAYERLIST-%1\n").arg(playerList).toUtf8();
    
    for (auto *c : clients) {
        c->write(msg);
        c->write(listMsg);
    }
    QTimer::singleShot(1000, this, &DummyServer::broadcastPlayers);
}

void DummyServer::broadcastMoveLogs() {
    // Wysyłamy całą historię głosów (allMoveLogs)
    QString logStr;
    for (const MoveVoteLog &log : allMoveLogs) {
        if (!logStr.isEmpty()) logStr += ";";
        logStr += QString("%1:%2:%3")
            .arg(log.teamColor, log.playerNick, log.move);
    }
    QByteArray msg = QString("MOVELOGS %1\n").arg(logStr).toUtf8();
    for (auto *c : clients) {
        c->write(msg);
    }
}

void DummyServer::startCountdown() {
    if (!countdownTimer.isActive()) {
        // Wyślij COUNTDOWN do wszystkich klientów
        QByteArray countdownMsg = "COUNTDOWN\n";
        for (auto *c : clients) {
            c->write(countdownMsg);
        }
        
        countdownTimer.setSingleShot(true);
        connect(&countdownTimer, &QTimer::timeout, this, &DummyServer::sendPlay);
        countdownTimer.start(5000); // 5 sekund
    }
}

void DummyServer::sendPlay() {
    
    for (auto *c : clients) {
        c->write("PLAY\n");
        c->write("BOARD-e2,d3,Ke8|a7,b6\n");
    }
    readyClients.clear();
    countdownTimer.disconnect();
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    DummyServer server;
    return a.exec();
}
