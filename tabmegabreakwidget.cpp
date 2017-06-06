#include "tabmegabreakwidget.h"
#include "ui_tabmegabreakwidget.h"

TabMegabreakWidget::TabMegabreakWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabMegabreakWidget)
{
    ui->setupUi(this);
}

TabMegabreakWidget::~TabMegabreakWidget()
{
    delete ui;
}

void TabMegabreakWidget::reset()
{

}

void TabMegabreakWidget::configure(ProjectSettings &settings)
{

}

void TabMegabreakWidget::updateCurrentSettings(ProjectSettings &settings)
{

}
