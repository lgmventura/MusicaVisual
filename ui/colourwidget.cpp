#include "colourwidget.h"

ColourWidget::ColourWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setAutoFillBackground(true);
}

// Other member functions:
void ColourWidget::setBackgroundColour(int r, int g, int b)
{
    QColor qColour;
    qColour.setRgb(r, g, b);
    this->setBackgroundColour(qColour);
}

void ColourWidget::setBackgroundColour(QColor qColour)
{
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, qColour);
    this->setPalette(pal);
}

QColor ColourWidget::getBackgroundColour()
{
    QPalette pal;
    QColor qcolour;
    pal = this->palette();
    qcolour = pal.color(QWidget::backgroundRole());
    return qcolour;
}

void ColourWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QColor qcolour;
        qcolour = this->getBackgroundColour();
        qcolour = QColorDialog::getColor(qcolour, this, "Pick a colour for the vertical rythmic lines", QColorDialog::DontUseNativeDialog);
        if (!qcolour.isValid()) return;
        this->setBackgroundColour(qcolour);

        emit colourChanged();
    }
}
