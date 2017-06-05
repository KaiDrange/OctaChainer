#ifndef TABGRIDWIDGET_H
#define TABGRIDWIDGET_H

#include <QWidget>

namespace Ui {
class TabGridWidget;
}

class TabGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabGridWidget(QWidget *parent = 0);
    ~TabGridWidget();

private:
    Ui::TabGridWidget *ui;
};

#endif // TABGRIDWIDGET_H
