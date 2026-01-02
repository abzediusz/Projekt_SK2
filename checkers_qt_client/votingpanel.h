#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class NetworkManager;

class VotingPanel : public QWidget {
    Q_OBJECT
public:
    explicit VotingPanel(NetworkManager *network, QWidget *parent = nullptr);
    
public slots:
    void onVoteResultsReceived(int opt1, int opt2, int opt3, int opt4);
    void onVotingEnded(int opt1, int opt2, int opt3, int opt4, const QString &winner);
    void onVoteConfirmed(int option);
    
private slots:
    void onVoteButton(int option);
    void onRefreshResults();
    void onClearVote();
    
private:
    NetworkManager *network;
    
    // Vote buttons
    QPushButton *voteButtons[4];  // 0-3 for options 1-4
    
    // Result labels
    QLabel *resultLabels[4];
    QLabel *statusLabel;
    
    int currentVote = 0;
    
    void updateResultDisplay(int opt1, int opt2, int opt3, int opt4);
};
