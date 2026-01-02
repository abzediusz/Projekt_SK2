
#include "checkerpiecebutton.h"
#include <QPainter>
#include <QString>
#include <QStyleOptionButton>
#include <QPainterPath>


CheckerPieceButton::CheckerPieceButton(PieceColor color, QWidget *parent)
    : QPushButton(parent), m_color(color) {
    setFlat(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::NoFocus);
    setCursor(Qt::PointingHandCursor);
    setCheckable(false);
}

void CheckerPieceButton::setPieceColor(PieceColor color) {
    m_color = color;
    update();
}

CheckerPieceButton::PieceColor CheckerPieceButton::pieceColor() const {
    return m_color;
}

void CheckerPieceButton::setKing(bool king) {
    m_king = king;
    update();
}

bool CheckerPieceButton::isKing() const {
    return m_king;
}

void CheckerPieceButton::setHighlight(Highlight h) {
    m_highlight = h;
    update();
}

CheckerPieceButton::Highlight CheckerPieceButton::highlight() const {
    return m_highlight;
}

void CheckerPieceButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Tło pola
    QStyleOptionButton opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &p, this);

    QPointF center = rect().center();
    qreal radius = qMin(width(), height()) * 0.38;

    // Highlight jako obramówka pionka
    if (m_highlight != NoHighlight && m_color != None) {
        QColor border;
        if (m_highlight == Yellow) border = QColor(255, 220, 40);
        else if (m_highlight == Green) border = QColor(80, 255, 80);
        else if (m_highlight == Red) border = QColor(255, 60, 60);
        QPen pen(border, 5);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(center, radius + 4, radius + 4);
    }

    // Szklany pionek na polu docelowym (glass effect)
    if (m_color == None && (m_highlight == Green || m_highlight == Red)) {
        QColor glass = (m_highlight == Green) ? QColor(80,255,80,90) : QColor(255,60,60,90);
        QRadialGradient grad(center, radius);
        grad.setColorAt(0.0, QColor(255,255,255,120));
        grad.setColorAt(0.7, glass);
        grad.setColorAt(1.0, QColor(180,180,180,60));
        p.setPen(Qt::NoPen);
        p.setBrush(grad);
        p.drawEllipse(center, radius, radius);
    }

    // Cień pod pionkiem (delikatniejszy, mniejszy)
    if (m_color != None) {
        QColor shadowColor(0, 0, 0, 30);
        p.setBrush(shadowColor);
        p.setPen(Qt::NoPen);
        p.drawEllipse(center + QPointF(0, radius * 0.12), radius * 0.85, radius * 0.22);
    }

    // Pionek - idealny okrąg
    if (m_color != None) {
        QRadialGradient grad(center, radius);
        if (m_color == White) {
            grad.setColorAt(0.0, QColor(255,255,255));
            grad.setColorAt(0.8, QColor(230,230,230));
            grad.setColorAt(1.0, QColor(180,180,180));
            p.setPen(QPen(QColor(180,180,180), 2));
        } else {
            grad.setColorAt(0.0, QColor(60,60,60));
            grad.setColorAt(0.8, QColor(30,30,30));
            grad.setColorAt(1.0, QColor(120,120,120));
            p.setPen(QPen(QColor(60,60,60), 2));
        }
        p.setBrush(grad);
        p.drawEllipse(center, radius, radius);
    }

    // Litera "Q" dla króla (damki)
    if (m_king && m_color != None) {
        QFont kingFont;
        kingFont.setPointSize(20);
        kingFont.setBold(true);
        p.setFont(kingFont);
        
        QColor textColor = (m_color == White) ? Qt::black : Qt::white;
        p.setPen(textColor);
        p.drawText(rect(), Qt::AlignCenter, "Q");
    }
}
