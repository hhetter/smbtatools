 
 

 #include "smbtamonitor-run.h"


  
 Smbtamonitor_run::~Smbtamonitor_run(){}

 Smbtamonitor_run::Smbtamonitor_run(QWidget *parent)
{
  
  
 // qDebug()<< "Class constructor Smbtamonitor_run";
}

void Smbtamonitor_run::run(){
  
 qDebug() << "Class Smbtamonitor_run";
 monitorprocess = new QProcess;
 monitorprocess->start("./owntools");
 
   timeclass = new Timeclass;
  timeclass->run();
//  connect(timeclass->timer, SIGNAL(timeout()), qApp, SLOT(quit()));
  connect(timeclass->timer, SIGNAL(timeout()), this, SLOT(smr_timersignal()));

}

void Smbtamonitor_run::smr_timersignal(){
   
  qDebug()<<"smr_timersignal"; 
  
}

#include "smbtamonitor-run.moc"
