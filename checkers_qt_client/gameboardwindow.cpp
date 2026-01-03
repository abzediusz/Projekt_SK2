#include "gameboardwindow.h"
#include "networkmanager.h"
#include "movevotingpanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QDebug>
#include <QMessageBox>

GameBoardWindow::GameBoardWindow(NetworkManager *net, QWidget *parent)
    : QWidget(parent), network(net)
{
    setWindowTitle("Warcaby - Gra");
    setFixedSize(790, 465);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Board
    setupBoard();
    mainLayout->addWidget(createBoardLayout(), 3);
    
    // Spacing
    mainLayout->addSpacing(10);
    
    // Voting panel
    moveVotingPanel = new MoveVotingPanel(network, this);
    moveVotingPanel->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(moveVotingPanel, 1);
    
    // Connect signals
    connect(network, &NetworkManager::boardUpdate, this, &GameBoardWindow::updateBoard);
    connect(network, &NetworkManager::teamColorSet, this, &GameBoardWindow::onTeamColorSet);
    connect(network, &NetworkManager::invalidMoveError, this, &GameBoardWindow::onInvalidMove);
}

QWidget* GameBoardWindow::createBoardLayout()
{
    QWidget *boardWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(boardWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    
    // Top labels (a-h)
    QHBoxLayout *topLabels = new QHBoxLayout();
    topLabels->setSpacing(0);
    topLabels->setContentsMargins(0, 0, 0, 5);
    topLabels->addSpacing(40);
    for (char c = 'a'; c <= 'h'; ++c) {
        QLabel *lbl = new QLabel(QString(c));
        lbl->setFixedSize(50, 20);
        lbl->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
        QFont f = lbl->font();
        f.setBold(true);
        lbl->setFont(f);
        topLabels->addWidget(lbl);
    }
    layout->addLayout(topLabels, 0);
    
    // Board with row labels
    QHBoxLayout *boardRow = new QHBoxLayout();
    boardRow->setSpacing(0);
    boardRow->setContentsMargins(0, 0, 0, 0);
    
    // Row numbers (8-1)
    QVBoxLayout *rowLabels = new QVBoxLayout();
    rowLabels->setSpacing(0);
    rowLabels->setContentsMargins(0, 0, 0, 0);
    for (int row = 0; row < 8; ++row) {
        QLabel *lbl = new QLabel(QString::number(8 - row));
        lbl->setFixedSize(40, 50);
        lbl->setAlignment(Qt::AlignCenter);
        QFont f = lbl->font();
        f.setBold(true);
        lbl->setFont(f);
        rowLabels->addWidget(lbl);
    }
    boardRow->addLayout(rowLabels);
    
    // Grid
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(0);
    grid->setContentsMargins(0, 0, 0, 0);
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QPushButton *btn = new QPushButton();
            btn->setFixedSize(50, 50);
            btn->setFlat(true);
            bool isLight = (row + col) % 2 == 0;
            btn->setStyleSheet(QString("background-color: %1; border: none;")
                .arg(isLight ? "#f0d9b5" : "#b58863"));
            
            grid->addWidget(btn, row, col);
            boardCells[row][col] = btn;
            
            connect(btn, &QPushButton::clicked, this, [this, row, col]() {
                onCellClicked();
            });
        }
    }
    boardRow->addLayout(grid);
    layout->addLayout(boardRow, 0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    
    return boardWidget;
}

void GameBoardWindow::setupBoard()
{
    // Initialized in createBoardLayout
}

void GameBoardWindow::updateBoard(const QStringList &white, const QStringList &black)
{
    qDebug() << "updateBoard: white=" << white << "black=" << black;
    
    whitePieces = white;
    blackPieces = black;
    
    // Clear all
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            boardCells[row][col]->setText("");
            boardCells[row][col]->setStyleSheet(QString("background-color: %1; border: none;")
                .arg(((row + col) % 2 == 0) ? "#f0d9b5" : "#b58863"));
        }
    }
    
    // Draw white pieces
    for (const QString &pieceOrig : white) {
        QString piece = pieceOrig.toUpper();
        QString cell = piece.right(2);
        int col = cell[0].toLatin1() - 'A';
        int row = 8 - cell[1].digitValue();
        
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            QString text = piece.startsWith("K") ? "◉" : "●";
            boardCells[row][col]->setText(text);
            boardCells[row][col]->setStyleSheet(QString(
                "background-color: %1; color: white; font-size: 45px; font-weight: bold; border: none;")
                .arg(((row + col) % 2 == 0) ? "#f0d9b5" : "#b58863"));
        }
    }
    
    // Draw black pieces
    for (const QString &pieceOrig : black) {
        QString piece = pieceOrig.toUpper();
        QString cell = piece.right(2);
        int col = cell[0].toLatin1() - 'A';
        int row = 8 - cell[1].digitValue();
        
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            QString text = piece.startsWith("K") ? "◉" : "●";
            boardCells[row][col]->setText(text);
            boardCells[row][col]->setStyleSheet(QString(
                "background-color: %1; color: black; font-size: 45px; font-weight: bold; border: none;")
                .arg(((row + col) % 2 == 0) ? "#f0d9b5" : "#b58863"));
        }
    }
}

void GameBoardWindow::onTeamColorSet(const QString &color)
{
    playerColor = color;
    qDebug() << "Team color set to:" << color;
}

void GameBoardWindow::onCellClicked()
{
    if (playerColor == "observer") return;
    
    // Find which button was clicked
    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    if (!sender) return;
    
    // Find coordinates
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (boardCells[row][col] == sender) {
                QString cell = coordToCell(row, col);
                
                if (selectedFrom.isEmpty()) {
                    // First click - select piece
                    bool isWhite = isWhitePiece(cell);
                    bool isBlack = isBlackPiece(cell);
                    
                    // Only allow selecting own team's pieces
                    if ((playerColor == "white" && isWhite) || (playerColor == "black" && isBlack)) {
                        selectedFrom = cell;
                        // Highlight - preserve the piece symbol and style
                        bool isK = isDama(cell);
                        QString symbol = isK ? "◉" : "●";
                        
                        boardCells[row][col]->setText(symbol);
                        boardCells[row][col]->setStyleSheet(QString(
                            "background-color: #ffff00; color: %1; font-size: 45px; font-weight: %2; border: 3px solid red;")
                            .arg(isWhite ? "white" : "black")
                            .arg(isK ? "bold" : "normal"));
                    }
                } else {
                    // Second click - make move
                    if (cell == selectedFrom) {
                        // Deselect
                        updateBoard(whitePieces, blackPieces);
                        selectedFrom.clear();
                    } else {
                        // Send vote - czekamy na potwierdzenie z serwera
                        QString move = QString("m-%1-%2").arg(selectedFrom.toUpper(), cell.toUpper());
                        network->sendMoveVote(move);
                        
                        // Clear selection
                        updateBoard(whitePieces, blackPieces);
                        selectedFrom.clear();
                    }
                }
                return;
            }
        }
    }
}

void GameBoardWindow::notifyVote(const QString &move)
{
    if (moveVotingPanel) {
        moveVotingPanel->onVoteSent(move);
    }
}

void GameBoardWindow::onInvalidMove()
{
    // Clear selection and show error
    selectedFrom.clear();
    updateBoard(whitePieces, blackPieces);
    
    QMessageBox::critical(this, "Niepoprawny głos", 
        "Twój głos został odrzucony przez serwer.\n"
        "Ruch jest niepoprawny lub nie jest Twoja kolej.\n\n"
        "Spróbuj ponownie.");
}

QString GameBoardWindow::coordToCell(int row, int col) const
{
    QChar letter = QChar('A' + col);
    int number = 8 - row;
    return QString("%1%2").arg(letter).arg(number);
}

bool GameBoardWindow::isWhitePiece(const QString &cell) const
{
    for (const QString &p : whitePieces) {
        if (p.endsWith(cell)) return true;
    }
    return false;
}

bool GameBoardWindow::isBlackPiece(const QString &cell) const
{
    for (const QString &p : blackPieces) {
        if (p.endsWith(cell)) return true;
    }
    return false;
}

bool GameBoardWindow::isDama(const QString &cell) const
{
    for (const QString &p : whitePieces) {
        if (p.startsWith("K") && p.endsWith(cell)) return true;
    }
    for (const QString &p : blackPieces) {
        if (p.startsWith("K") && p.endsWith(cell)) return true;
    }
    return false;
}
