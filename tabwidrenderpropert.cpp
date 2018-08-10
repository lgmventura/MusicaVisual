#include "tabwidrenderpropert.h"
#include "ui_tabwidrenderpropert.h"

TabWidRenderPropert::TabWidRenderPropert(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidRenderPropert)
{
    ui->setupUi(this);
}

TabWidRenderPropert::~TabWidRenderPropert()
{
    delete ui;
}
