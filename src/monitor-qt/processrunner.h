#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QWidget>
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QDebug>
//#include <QLocalSocket>
#include <QApplication>
#include "instancedata.h"

class Processrunner : public QObject
{
    Q_OBJECT
public:
    explicit Processrunner(InstanceData *idata, QWidget *parent=0);

    QProcess *monitorprocess;


signals:

public slots:

    void pr_readsignal();

};

#endif // PROCESSRUNNER_H
