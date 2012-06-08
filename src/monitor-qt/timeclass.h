#ifndef TIMECLASS_H
#define TIMECLASS_H

#include <QThread>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QDebug>

class Timeclass : public QObject
{
    Q_OBJECT
public:
    explicit Timeclass(QWidget *parent = 0);
    QTimer *timer;

    void run();

signals:

public slots:

    void tc_timersignal();

};

#endif // TIMECLASS_H
