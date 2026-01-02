#pragma once
#include <QWidget>
#include <QList>

class NetworkManager;
class QLabel;
class QListWidget;
class QTableWidget;
class QPushButton;
struct MoveVote;

class MoveVotingPanel : public QWidget {
    Q_OBJECT
public:
    explicit MoveVotingPanel(NetworkManager *net, QWidget *parent = nullptr);
    void onVoteSent(const QString &move);
    
public slots:
    void onTeamColorSet(const QString &color);
    void onMoveLogsReceived(const QList<MoveVote> &logs);
    void onCancelVote();
    
private:
    NetworkManager *network;
    QString teamColor = "white";
    QString currentVote = ""; // Track current player's vote
    
    QLabel *teamLabel;
    QTableWidget *topMovesTable;
    QListWidget *logList;
    QPushButton *cancelButton;
    
    QString formatMove(const QString &move) const;
};
