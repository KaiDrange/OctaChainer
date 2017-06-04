#ifndef TABMAINWIDGET_H
#define TABMAINWIDGET_H

#include <QWidget>

namespace Ui {
class TabMainWidget;
}

class TabMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabMainWidget(QWidget *parent = 0);
    ~TabMainWidget();

private:
    Ui::TabMainWidget *ui;
};

#endif // TABMAINWIDGET_H
