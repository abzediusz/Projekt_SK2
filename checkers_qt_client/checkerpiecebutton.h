#pragma once
#include <QPushButton>

class CheckerPieceButton : public QPushButton {
    Q_OBJECT
public:
    enum PieceColor { None, White, Black };
    enum Highlight { NoHighlight, Yellow, Green, Red };
    CheckerPieceButton(PieceColor color = None, QWidget *parent = nullptr);
    void setPieceColor(PieceColor color);
    PieceColor pieceColor() const;
    void setKing(bool king);
    bool isKing() const;
    void setHighlight(Highlight h);
    Highlight highlight() const;
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    PieceColor m_color;
    bool m_king = false;
    Highlight m_highlight = NoHighlight;
};
