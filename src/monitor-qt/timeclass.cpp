#include "timeclass.h"

Timeclass::Timeclass(InstanceData *idata, QWidget *parent) :
    QObject(parent)
{

    //qDebug() << "Class constructor Timeclass";

    i_stepsize = idata->i_stepsize;
    //i_stepsize = 5;
    
    timer = new QTimer;
 //   timer->setInterval(60000/i_stepsize); // minute
   timer->setInterval(1000/i_stepsize);   // second, standard value
   // timer->setInterval(10000/i_stepsize);   // debug value
    //timer->setInterval(1400);   //debug times
    //timer->setInterval(300/i_stepsize);   // second
//    timer->setInterval(60/i_stepsize);	// milisecond
 //   timer->start();

     connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));

}


void Timeclass::tc_timersignal(){

  qDebug()<<"timer";

}



#include "timeclass.moc"
