#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QWidget>
#include <QThread>

class MonitorWidget : public QThread
{
    Q_OBJECT
public:
    explicit MonitorWidget(QWidget *parent = 0);



signals:

public slots:

};

#endif // MONITORWIDGET_H
