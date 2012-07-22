#ifndef MONITORFORM_H
#define MONITORFORM_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QLabel>
#include <QProcess>
#include <QFile>
#include <QString>
#include <QHBoxLayout>
#include <QMutex>

//#include <QtNetwork>
#include <QLocalSocket>

#include <QApplication>
#include <QProcess>

#include "processrunner.h"
#include "timeclass.h"
#include "visual.h"
#include "dpoint.h"
#include "graph.h"

namespace Ui {
    class MonitorForm;
}

class MonitorForm : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorForm(QWidget *parent = 0);
    ~MonitorForm();

    Timeclass     *timeClassW;
    Processrunner *processRunnerW;
    Visual *visualW;
    DPoint *d_points;
    Graph *p_graph;

    QMutex mutex;

    int i_monitortime, i_intervaltime;
    unsigned long *l_writestack, *l_readstack;
    bool running, layouted;


    QString  *pid_string, *readstring, *leftoverstring, *configString;
    QStringList *readlist;
    QLocalSocket *monitorSocket;


public slots:

    void startmonitor();
    void stopmonitor();
    void readfromsocket();
    void sendmessage();
    void sendtovisualizer();
    void sendtopoints();
    void parseinput();
    void resizeEvent(QResizeEvent *event);

private:
    Ui::MonitorForm *ui;
};

#endif // MONITORFORM_H
