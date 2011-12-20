 
 

 #include "smbtamonitor-run.h"


  
 Smbtamonitor_run::~Smbtamonitor_run(){}

 Smbtamonitor_run::Smbtamonitor_run(QWidget *parent)
{
  
  i_debug=0;
  l_writestack = new unsigned long; *l_writestack = 0;
  l_readstack = new  unsigned long;  *l_readstack = 0;
  output = new QString;


  smbtalayout = new QVBoxLayout;
  smbtawidget = new QWidget;
  visualizer = new Visual( smbtawidget, 600);
  processrunner = new Processrunner;
  timeclass = new Timeclass;
  runtestline = new QLabel("QLabel Smbtamonitor_run class testline", smbtawidget);
  smbtalayout->addWidget(runtestline,0);
  smbtawidget->setLayout(smbtalayout);
 qDebug()<< "Class constructor Smbtamonitor_run";
 
 
}

void Smbtamonitor_run::run(){
  

  
//   qDebug()<< "Smbtamonitor_run  1";
  processrunner->run();
//  processrunner->start();
//   qDebug()<< "Smbtamonitor_run  2";
  connect(processrunner->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(smr_sendmessage()));

//   qDebug()<< "Smbtamonitor_run  3";  

  timeclass->run();
//  timeclass->start();
  connect(timeclass->timer, SIGNAL(timeout()), this, SLOT(smr_timersignal()));

  

//  visualizer->visuallabel->setText("Jetzt aber.");
//  visualizer->visualwidget->show();
  smbtalayout->addWidget(visualizer,1);
  smbtawidget->show();

  
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

  mutex.lock();
  
//  qDebug() << "++++++++++++++++++++++++++++++++++++++++";
//  qDebug()<<"smr_timersignal";
//  *visualizer->l_visualread  = *l_readstack;
//  *visualizer->l_visualwrite = *l_writestack;
//  qDebug() << "*l_readstack  : " << *l_readstack;
//  qDebug() << "*l_writestack  : " << *l_writestack;  


  visualizer->vs_processnumbers(l_readstack, l_writestack);
 
  
  *l_readstack  =  0;
  *l_writestack = 0;
//  qDebug() << "---------------------------------------------------";
//  qDebug(); qDebug();
  
  mutex.unlock();
}

void Smbtamonitor_run::smr_visualizer(){
   
  qDebug()<<"smr_visualizer"; 
  
}

void Smbtamonitor_run::smr_sendmessage(){
 

  *output = processrunner->monitorprocess->readLine();
  smr_parsemonitor();
//  qDebug() << *output;
     
  
}

void Smbtamonitor_run::smr_parsemonitor(){
  
//  qDebug() << "Smbtamonitor_run::smr_parsemonitor()";
  
  ////
  // Adjustet output parse lines to work with remote test smbta host
  if( output->startsWith("W")){
    output->replace("W: ","");
     *l_writestack += output-> toULong();
//     qDebug() << "output: "  << output->toULong();  
  }
  
  if( output->startsWith("R")){
    output->replace("R: ","");
     *l_readstack += output->toULong();
  }
  
  runtestline->setText(*output);
  
}

#include "smbtamonitor-run.moc"
