#include "tabmainwidget.h"
#include "ui_tabmainwidget.h"

TabMainWidget::TabMainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabMainWidget)
{
    ui->setupUi(this);
}

TabMainWidget::~TabMainWidget()
{
    delete ui;
}
