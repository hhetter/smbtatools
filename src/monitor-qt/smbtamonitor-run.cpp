 
 

 # include "smbtamonitor-run.h"


  
 Smbtamonitor_run::~Smbtamonitor_run(){}

 Smbtamonitor_run::Smbtamonitor_run(QWidget *parent)
{
  
  
 // qDebug()<< "Hurz";
}

void Smbtamonitor_run::run(){
  
 qDebug() << "run, run!";
 monitorprocess = new QProcess;
// connect(monitorprocess, SIGNAL(readyReadStandardOutput()), this,SLOT(sr_emit()));

 monitorprocess->start("./owntools >> owntools.txt");
 
 qDebug() << "run another time!";
}

void Smbtamonitor_run::sr_emit(){
  
  qDebug("runner");
  
}