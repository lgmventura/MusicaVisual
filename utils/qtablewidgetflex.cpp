#include "qtablewidgetflex.h"



QTableWidgetFlex::QTableWidgetFlex()
{

}

// modified from: https://www.qtcentre.org/threads/3386-QTableWidget-move-row
void QTableWidgetFlex::moveRows1Step(bool up)
{
    //Q_ASSERT(selectedItems().count() > 0);
    const int sourceRow = currentRow();
    const int destRow = (up ? sourceRow-1 : sourceRow+1);
    //Q_ASSERT(destRow >= 0 && destRow < rowCount());

    // take whole rows
    QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
    QList<QTableWidgetItem*> destItems = takeRow(destRow);

    // set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);
}

void QTableWidgetFlex::moveRow(int fromPos, int toPos)
{
    Q_ASSERT(selectedItems().count() == 1);
    Q_ASSERT(toPos >= 0 && toPos < rowCount());

    // take whole rows
    QList<QTableWidgetItem*> sourceItems = takeRow(fromPos);
    QList<QTableWidgetItem*> destItems = takeRow(toPos);

    // set back in reverse order
    setRow(fromPos, destItems);
    setRow(toPos, sourceItems);
}

// takes and returns the whole row
QList<QTableWidgetItem*> QTableWidgetFlex::takeRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0; col < columnCount(); ++col)
    {
        rowItems << takeItem(row, col);
    }
    return rowItems;
}

// sets the whole row
void QTableWidgetFlex::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int col = 0; col < columnCount(); ++col)
    {
        setItem(row, col, rowItems.at(col));
    }
}
