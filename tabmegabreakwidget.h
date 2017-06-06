#ifndef TABMEGABREAKWIDGET_H
#define TABMEGABREAKWIDGET_H

#include <QWidget>
#include "projectsettings.h"

extern QString _defaultPathAudio;
extern QString _defaultPathProjects;
extern QString _defaultPathOutput;

namespace Ui {
class TabMegabreakWidget;
}

class TabMegabreakWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabMegabreakWidget(QWidget *parent = 0);
    ~TabMegabreakWidget();
    void reset();
    void configure(ProjectSettings &settings);
    void updateCurrentSettings(ProjectSettings &settings);

private:
    Ui::TabMegabreakWidget *ui;
};

#endif // TABMEGABREAKWIDGET_H
