#include "renderpropert.h"
#include "ui_renderpropert.h"

#include "mainwindow.h"

RenderPropert::RenderPropert(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::RenderPropert)
{
    ui->setupUi(this);
}

RenderPropert::~RenderPropert()
{
    delete ui;
}

extern RenderP *renderproperties;


void RenderPropert::on_checkBox_toggled(bool checked)
{
    renderproperties->lines[0] = checked;
}

void RenderPropert::on_checkBox_2_toggled(bool checked)
{
    renderproperties->lines[1] = checked;
}
