#ifndef PROCESSRUNNER_H
#define PROCESSRUNNER_H

#include <QWidget>
#include <QThread>
#include <QProcess>
#include <QDebug>

class Processrunner : public QThread
{
    Q_OBJECT
public:
    explicit Processrunner(QWidget *parent = 0);

    QProcess *monitorprocess;

    void run();

signals:

public slots:

    void pr_readsignal();

};

#endif // PROCESSRUNNER_H
