#ifndef MONITORFORM_H
#define MONITORFORM_H

#include <QWidget>
#include <QThread>
#include <QMutex>

#include "processrunner.h"
#include "timeclass.h"

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

    QMutex mutex;

    int i_debug;
    unsigned long *l_writestack, *l_readstack;


public slots:

    void startmonitor();
    void sendmessage();

private:
    Ui::MonitorForm *ui;
};

#endif // MONITORFORM_H
