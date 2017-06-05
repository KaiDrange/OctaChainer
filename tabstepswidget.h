#ifndef TABSTEPSWIDGET_H
#define TABSTEPSWIDGET_H

#include <QWidget>

namespace Ui {
class TabStepsWidget;
}

class TabStepsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabStepsWidget(QWidget *parent = 0);
    ~TabStepsWidget();

private:
    Ui::TabStepsWidget *ui;
};

#endif // TABSTEPSWIDGET_H
