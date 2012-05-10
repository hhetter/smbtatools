#ifndef MONITORFORM_H
#define MONITORFORM_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QLabel>
#include <QProcess>
#include <QFile>
#include <QString>

#include <QtNetwork>
#include <QLocalSocket>

#include <QApplication>

#include "processrunner.h"
#include "timeclass.h"
#include "visual.h"

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

    QMutex mutex;

    int i_debug;
    unsigned long *l_writestack, *l_readstack;
    bool running;


    QString  *pid_string, *readstring;
    QStringList *readlist;
    QLocalSocket *monitorSocket;


public slots:

    void startmonitor();
    void stopmonitor();
    void readfromsocket();
    void sendmessage();
    void sendtovisualizer();
    void parseinput();

private:
    Ui::MonitorForm *ui;
};

#endif // MONITORFORM_H
