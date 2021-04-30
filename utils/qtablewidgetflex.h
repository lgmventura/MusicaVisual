#ifndef QTABLEWIDGETFLEX_H
#define QTABLEWIDGETFLEX_H

#include <QTableWidget>

class QTableWidgetFlex : public QTableWidget
{
public:
    QTableWidgetFlex();

    void moveRows1Step(bool up);
    void moveRow(int from, int to);

    QList<QTableWidgetItem*> takeRow(int row);
    void setRow(int row, const QList<QTableWidgetItem*>& rowItems);
};

#endif // QTABLEWIDGETFLEX_H
