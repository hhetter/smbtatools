#ifndef WMANAGER_H
#define WMANAGER_H

#include <QWidget>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QLabel>
#include <QTextEdit>
#include <QMdiArea>

#include <QMainWindow>
#include <QMdiSubWindow>

#include "configform.h"
#include "monitorform.h"
#include "frontend.h"
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
    frontend *newFrontendArray[128];
    frontend *newFrontend;

    QGraphicsScene *scene;



public slots:
    void wm_newMonitorWidget();
    void wm_firstInit();

    void resizeEvent(QResizeEvent *event);

    
private:
    Ui::WManager *ui;





};

#endif // WMANAGER_H
