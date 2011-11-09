 
 

 #include "smbtamonitor-run.h"


  
 Smbtamonitor_run::~Smbtamonitor_run(){}

 Smbtamonitor_run::Smbtamonitor_run(QWidget *parent)
{
  
  i_debug=0;
  outputline = new QLabel("QLabel outputline");
 
 qDebug()<< "Class constructor Smbtamonitor_run";
}

void Smbtamonitor_run::run(){
  
   qDebug()<< "Smbtamonitor_run  1";
  processrunner = new Processrunner;
  processrunner->run();
   qDebug()<< "Smbtamonitor_run  2";
  connect(processrunner->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(smr_sendmessage()));

   qDebug()<< "Smbtamonitor_run  3";  
  timeclass = new Timeclass;
  timeclass->run();
//  connect(timeclass->timer, SIGNAL(timeout()), qApp, SLOT(quit()));

  
  visualizer = new Visual;
  
//    sleep(1);
//    connect(processrunner->monitorprocess, SIGNAL(readyReadStandardOutput()), qApp, SLOT(quit()));
//    connect(timeclass->timer, SIGNAL(timeout()), this, SLOT(smr_timersignal()));
    
    
    ////
    // Only for debugging reasons
//   monitorprocess2 = new QProcess; 
//   qDebug() << "debugging reasons";
//   monitorprocess2->start("./owntools");
//   connect(monitorprocess2, SIGNAL(readyReadStandardOutput()), qApp, SLOT(quit()));
//   connect(monitorprocess2, SIGNAL(readyReadStandardOutput()), this, SLOT(smr_visualizer()));

}

void Smbtamonitor_run::smr_timersignal(){
   
  qDebug()<<"smr_timersignal"; 
  
}

void Smbtamonitor_run::smr_visualizer(){
   
  qDebug()<<"smr_visualizer"; 
  
}

void Smbtamonitor_run::smr_sendmessage(){
      qDebug()<< "Smbtamonitor_run  4";
  i_debug++;
  qDebug()<<i_debug;
  if(i_debug == 1000000){
   processrunner->monitorprocess->close();
//   qDebug() << "fin";
//   i_debug=0;
    
  }
  output = new QString;
  *output = processrunner->monitorprocess->readLine();
  smr_parsemonitor();
//  qDebug() << *output;
     
  
}

void Smbtamonitor_run::smr_parsemonitor(){
  
  if( output->startsWith("W")){
    qDebug() << "Write" << *output;
    output->replace("W: ","");
//    qDebug()<< "Replaced .." << *output; 
    int i_out = output->toInt();
    qDebug() << i_out;
  }
  
  if( output->startsWith("R")){
    qDebug() << "Read";
  }
  
  outputline->setText(*output);
  
}

#include "smbtamonitor-run.moc"
