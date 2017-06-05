#include "tabstepswidget.h"
#include "ui_tabstepswidget.h"

TabStepsWidget::TabStepsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabStepsWidget)
{
    ui->setupUi(this);
}

TabStepsWidget::~TabStepsWidget()
{
    delete ui;
}
