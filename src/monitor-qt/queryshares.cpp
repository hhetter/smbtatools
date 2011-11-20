 
 

 #include "queryshares.h"


  
 Queryshares::~Queryshares(){}

 Queryshares::Queryshares(QWidget *parent)
{
   qDebug() << " Constructor Class Queryshares";

}

void Queryshares::run(){

  
   queryprocess = new QProcess; 
   qDebug() << "Queryshares::run";
   queryprocess->start("./smbtaquery");
  
   qs_readsignal();

}


void Queryshares::qs_readsignal(){
   
//  qDebug()<<"pr_readsignal"; 
  
}

#include "queryshares.moc"
