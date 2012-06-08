#include "timeclass.h"

Timeclass::Timeclass(QWidget *parent) :
    QObject(parent)
{

    qDebug() << "Class constructor Timeclass";

    timer = new QTimer;
 //   timer->setInterval(60000/5); // minute
    timer->setInterval(600/5);   // second
 //   timer->setInterval(60/5);	// milisecond
 //   timer->start();

     connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));

}


void Timeclass::tc_timersignal(){

  qDebug()<<"timer";

}


#include "timeclass.moc"
