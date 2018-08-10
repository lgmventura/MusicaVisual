#ifndef TABWIDRENDERPROPERT_H
#define TABWIDRENDERPROPERT_H

#include <QTabWidget>

namespace Ui {
class TabWidRenderPropert;
}

class TabWidRenderPropert : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidRenderPropert(QWidget *parent = 0);
    ~TabWidRenderPropert();

private:
    Ui::TabWidRenderPropert *ui;
};

#endif // TABWIDRENDERPROPERT_H
