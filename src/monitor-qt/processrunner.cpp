 
 

 #include "processrunner.h"


  
 Processrunner::~Processrunner(){}

 Processrunner::Processrunner(QWidget *parent)
{
   qDebug() << " Constructor Class Processrunner";

}

void Processrunner::run(){

  
   monitorprocess = new QProcess; 
   qDebug() << "Processrunner::run";
   monitorprocess->start("./owntools");
//   connect(monitorprocess, SIGNAL(readyReadStandardOutput()), qApp, SLOT(quit()));
   connect(monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(pr_readsignal()));
 

}


void Processrunner::Processrunner::pr_readsignal(){
   
  qDebug()<<"pr_readsignal"; 
  
}

#include "processrunner.moc"
