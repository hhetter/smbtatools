#ifndef FRONTEND_H
#define FRONTEND_H

#include <QMainWindow>

#include <QDebug>

#include <configuration.h>
#include <configform.h>
#include <monitorform.h>
#include <monitorwidget.h>


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
    MonitorWidget *monitorOldW;
    MonitorForm *monitorFormW;

public slots:
    void fr_setTestLabel();
    void fr_setConfigWidget();
    void fr_setMonitorWidget();

private:
    Ui::frontend *ui;
};

#endif // FRONTEND_H
