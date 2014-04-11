#ifndef FRONTEND_H
#define FRONTEND_H

#include <QMainWindow>
#include <QGraphicsWidget>
#include <QWidget>

#include <QDir>
#include <QDebug>

#include "monitorform.h"
//#include "wmanager.h"
#include "instancedata.h"

namespace Ui {
    class frontend;
}

class frontend : public QWidget
//class frontend : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit frontend(InstanceData *idata, QWidget *parent = 0);
    ~frontend();

    InstanceData *ldata;
    MonitorForm *monitorFormW;
//    WManager  *wManager;


public slots:
    void fr_setTestLabel();
    void fr_setMonitorWidget();
    void fr_firstInit();

    void resizeEvent(QResizeEvent *event);

private:
    Ui::frontend *ui;
};

#endif // FRONTEND_H
