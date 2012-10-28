#ifndef FRONTEND_H
#define FRONTEND_H

#include <QMainWindow>

#include <QDir>
#include <QDebug>

#include "configform.h"
#include "monitorform.h"
//#include "wmanager.h"
#include "instancedata.h"

namespace Ui {
    class frontend;
}

class frontend : public QMainWindow
{
    Q_OBJECT

public:
    explicit frontend(QWidget *parent = 0);
    ~frontend();

    InstanceData *idata;
    ConfigForm *configWidget;
    MonitorForm *monitorFormW;
//    WManager  *wManager;


public slots:
    void fr_setTestLabel();
    void fr_setConfigWidget();
    void fr_setMonitorWidget();
    void fr_firstInit();

    void resizeEvent(QResizeEvent *event);

private:
    Ui::frontend *ui;
};

#endif // FRONTEND_H
