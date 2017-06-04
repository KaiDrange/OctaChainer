#ifndef TABMEGABREAKWIDGET_H
#define TABMEGABREAKWIDGET_H

#include <QWidget>

namespace Ui {
class TabMegabreakWidget;
}

class TabMegabreakWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabMegabreakWidget(QWidget *parent = 0);
    ~TabMegabreakWidget();

private:
    Ui::TabMegabreakWidget *ui;
};

#endif // TABMEGABREAKWIDGET_H
