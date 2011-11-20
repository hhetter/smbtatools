 
 

 # include "timeclass.h"


  
 Timeclass::~Timeclass(){}

 Timeclass::Timeclass(QWidget *parent)
{
   qDebug() << "Class Timeclass";

  
}

void Timeclass::run(){
  
   timer = new QTimer; 
//   timer->setInterval(60000); // minute
   timer->setInterval(600);   // second
//   timer->setInterval(60);	// milisecond
   timer->start();
   
    connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));

}


void Timeclass::tc_timersignal(){
  
//  qDebug()<<"timer";
  
}
  

#include "timeclass.moc"
