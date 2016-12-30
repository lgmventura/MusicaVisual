#ifndef RENDERPROPERT_H
#define RENDERPROPERT_H

#include <QWidget>
#include <QTabWidget>

namespace Ui {
class RenderPropert;
}

class RenderPropert : public QTabWidget
{
    Q_OBJECT

public:
    explicit RenderPropert(QWidget *parent = 0);
    ~RenderPropert();

private slots:
    void on_checkBox_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

private:
    Ui::RenderPropert *ui;
};

#endif // RENDERPROPERT_H
