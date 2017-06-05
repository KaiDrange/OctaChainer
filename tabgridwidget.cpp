#include "tabgridwidget.h"
#include "ui_tabgridwidget.h"

TabGridWidget::TabGridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabGridWidget)
{
    ui->setupUi(this);
}

TabGridWidget::~TabGridWidget()
{
    delete ui;
}
