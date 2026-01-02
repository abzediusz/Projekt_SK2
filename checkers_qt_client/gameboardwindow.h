#pragma once
#include <QWidget>
#include <QString>
#include <QStringList>

class NetworkManager;
class QGridLayout;
class QVBoxLayout;
class MoveVotingPanel;
class QPushButton;

class GameBoardWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameBoardWindow(NetworkManager *net, QWidget *parent = nullptr);
    
public slots:
    void updateBoard(const QStringList &white, const QStringList &black);
    
private slots:
    void onTeamColorSet(const QString &color);
    void onCellClicked();
    void onInvalidMove();
    
private:
    NetworkManager *network;
    MoveVotingPanel *moveVotingPanel;
    QString playerColor = "white";
    QString selectedFrom;
    QStringList whitePieces, blackPieces;
    
    // Board cells - 64 buttons dla każdego pola
    QPushButton *boardCells[8][8];
    
    void setupBoard();
    void drawPieces();
    void notifyVote(const QString &move);
    QWidget* createBoardLayout();
    QString coordToCell(int row, int col) const;
    bool isWhitePiece(const QString &cell) const;
    bool isBlackPiece(const QString &cell) const;
    bool isDama(const QString &cell) const;
};
