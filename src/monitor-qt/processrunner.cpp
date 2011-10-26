 
 

 #include "processrunner.h"


  
 Processrunner::~Processrunner(){}

 Processrunner::Processrunner(QWidget *parent)
{
  
  
 // qDebug()<< "Class constructor Processrunner";
}

void Processrunner::run(){
  
 qDebug() << "Class Processrunner";
 monitorprocess = new QProcess;
 monitorprocess->start("./owntools");
 
//   timeclass = new Timeclass;
//  timeclass->run();
//  connect(timeclass->timer, SIGNAL(timeout()), qApp, SLOT(quit()));
//  connect(timeclass->timer, SIGNAL(timeout()), this, SLOT(smr_timersignal()));

}

/*
void Processrunner::smr_timersignal(){
   
  qDebug()<<"smr_timersignal"; 
  
}
*/
#include "processrunner.moc"
