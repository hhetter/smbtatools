 
 

 # include "timeclass.h"


  
 Timeclass::~Timeclass(){}

 Timeclass::Timeclass(QWidget *parent)
{
  
  
}

void Timeclass::run(){
  
 qDebug() << "Class Timeclass";
 timer = new QTimer;
 timer->setInterval(60000);
 timer->start();

}

#include "timeclass.moc"
