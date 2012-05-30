#include "timeclass.h"

Timeclass::Timeclass(QWidget *parent) :
    QThread(parent)
{

    qDebug() << "Class constructor Timeclass";



//    timer = new QTimer;
 //   timer->setInterval(60000); // minute
//    timer->setInterval(600);   // second
 //   timer->setInterval(60);	// milisecond
 //   timer->start();

//     connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));

}

void Timeclass::run(){

//    qDebug()<<"Timeclass::run()";

   timer = new QTimer;
//   timer->setInterval(60000); // minute
//   timer->setInterval(1800);   // 3 seconds
//   timer->setInterval(600);   // second
   timer->setInterval(60);	// milisecond
   timer->start();

//    connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));
   exec();

}

void Timeclass::tc_timersignal(){

  qDebug()<<"timer";

}


#include "timeclass.moc"
