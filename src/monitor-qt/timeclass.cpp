#include "timeclass.h"

Timeclass::Timeclass(QWidget *parent) :
    QObject(parent)
{

    qDebug() << "Class constructor Timeclass";

    i_stepsize = 5;

    timer = new QTimer;
 //   timer->setInterval(60000/i_stepsize); // minute
    timer->setInterval(600/i_stepsize);   // second
    //timer->setInterval(100);   //debug times
//    timer->setInterval(300/i_stepsize);   // second
//    timer->setInterval(60/i_stepsize);	// milisecond
 //   timer->start();

     connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));

}


void Timeclass::tc_timersignal(){

  qDebug()<<"timer";

}



#include "timeclass.moc"
