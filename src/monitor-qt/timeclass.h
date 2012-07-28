#ifndef TIMECLASS_H
#define TIMECLASS_H

#include <QThread>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QDebug>

#include "instancedata.h"

class Timeclass : public QObject
{
    Q_OBJECT
public:
    explicit Timeclass(InstanceData *idata, QWidget *parent = 0);

    int i_stepsize; // i_stepsize defines the number of steps that are used to display a single data point

    QTimer *timer;



signals:

public slots:

    void tc_timersignal();

};

#endif // TIMECLASS_H
