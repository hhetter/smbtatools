 
 

 # include "timeclass.h"


  
 Timeclass::~Timeclass(){}

 Timeclass::Timeclass(QWidget *parent)
{
   qDebug() << "Class Timeclass";

  
}

void Timeclass::run(){
  
   timer = new QTimer; 
//   timer->setInterval(60000);
   timer->setInterval(600);
   timer->start();
   
    connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));

}


void Timeclass::tc_timersignal(){
  
//  qDebug()<<"timer";
  
}
  

#include "timeclass.moc"
