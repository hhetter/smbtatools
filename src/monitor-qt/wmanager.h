#ifndef WMANAGER_H
#define WMANAGER_H

#include <QWidget>
#include <QDebug>

#include "configform.h"
#include "monitorform.h"
#include "instancedata.h"


namespace Ui {
class WManager;
}

class WManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit WManager(QWidget *parent = 0);
    ~WManager();

    InstanceData *idata;
    ConfigForm *configWidget;
    MonitorForm *monitorFormW;


public slots:
    void wm_newMonitorWidget();

    
private:
    Ui::WManager *ui;





};

#endif // WMANAGER_H
