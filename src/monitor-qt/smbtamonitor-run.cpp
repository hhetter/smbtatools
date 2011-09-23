 
 

 # include "smbtamonitor-run.h"


  
 Smbtamonitor_run::~Smbtamonitor_run(){}

 Smbtamonitor_run::Smbtamonitor_run(QWidget *parent)
{
  
  
 // qDebug()<< "Hurz";
}

void Smbtamonitor_run::run(){
  
 qDebug() << "run, run!";
 monitorprocess = new QProcess;
 monitorprocess->start("./owntools");
 qDebug() << "run another time!";
}