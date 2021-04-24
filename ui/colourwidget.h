#ifndef COLOURWIDGET_H
#define COLOURWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QColorDialog>

class ColourWidget : public QWidget
{
    Q_OBJECT

public:
    ColourWidget(QWidget *parent = nullptr);

    void setBackgroundColour(int r, int g, int b);
    void setBackgroundColour(QColor qcolour);
    QColor getBackgroundColour();

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void colourChanged();
};

#endif // COLOURWIDGET_H
