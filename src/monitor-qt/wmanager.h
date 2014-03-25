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
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QMainWindow>
#include <QMdiSubWindow>

#include "configform.h"
#include "monitorform.h"
#include "frontend.h"
#include "instancedata.h"
#include "pgformular.h"

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

    QSqlDatabase db;
    QSqlQuery query;
    QString s_path, cf_path, df_path;
    pgformular *newPgformularArray[128];



public slots:
    void wm_newMonitorWidget();
    void wm_firstInit();
    void wm_init();
    void wm_pastMonitoWidget();

    void resizeEvent(QResizeEvent *event);

    
private:
    Ui::WManager *ui;





};

#endif // WMANAGER_H
