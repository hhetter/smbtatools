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
#include <QProgressDialog>
//#include <QTest>
#include <QMessageBox>

//#include <QtNetwork>
#include <QLocalSocket>

#include <QApplication>
#include <QProcess>

#include "processrunner.h"
#include "timeclass.h"
#include "dpoint.h"
#include "graph.h"
#include "instancedata.h"

namespace Ui {
class MonitorForm;
}

class MonitorForm : public QWidget
{
        Q_OBJECT

public:
        explicit MonitorForm(InstanceData *idata, QWidget *parent = 0);
        ~MonitorForm();


        InstanceData  *ldata;
        Timeclass     *timeClassW;
        Processrunner *processRunnerW;
        DPoint        *d_points;
        Graph         *p_graph;

        QMutex mutex;

        int i_monitortime, i_intervaltime;
        unsigned long *l_writestack, *l_readstack;
        bool running, layouted;
        bool isConnected;


        QString  *pid_string, *readstring, *leftoverstring, *configString;
        QString  *hostString, *portString, *shareString, *userString,
        *domainString, *fileString;
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
        void connectSocket();

        void resizeEvent(QResizeEvent *event);

private:
        Ui::MonitorForm *ui;
};

#endif // MONITORFORM_H
