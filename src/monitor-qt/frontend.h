#ifndef FRONTEND_H
#define FRONTEND_H

#include <QMainWindow>

#include <QDebug>

#include "configform.h"
#include "monitorform.h"


namespace Ui {
    class frontend;
}

class frontend : public QMainWindow
{
    Q_OBJECT

public:
    explicit frontend(QWidget *parent = 0);
    ~frontend();

    ConfigForm *configWidget;
    MonitorForm *monitorFormW;

public slots:
    void fr_setTestLabel();
    void fr_setConfigWidget();
    void fr_setMonitorWidget();
    void resizeEvent(QResizeEvent *event);

private:
    Ui::frontend *ui;
};

#endif // FRONTEND_H
