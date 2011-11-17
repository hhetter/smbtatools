 
 

 #include "smbtamonitor-run.h"


  
 Smbtamonitor_run::~Smbtamonitor_run(){}

 Smbtamonitor_run::Smbtamonitor_run(QWidget *parent)
{
  
  i_debug=0;
  l_writestack = new unsigned long; *l_writestack = 0;
  l_readstack = new  unsigned long;  *l_readstack = 0;
  smbtalayout = new QVBoxLayout;
  smbtawidget = new QWidget;
  runtestline = new QLabel("QLabel Smbtamonitor_run class testline", smbtawidget);
  smbtalayout->addWidget(runtestline,0);
  smbtawidget->setLayout(smbtalayout);
 qDebug()<< "Class constructor Smbtamonitor_run";
 
 
}

void Smbtamonitor_run::run(){
  

  
//   qDebug()<< "Smbtamonitor_run  1";
  processrunner = new Processrunner;
  processrunner->run();
//   qDebug()<< "Smbtamonitor_run  2";
  connect(processrunner->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(smr_sendmessage()));

//   qDebug()<< "Smbtamonitor_run  3";  
  timeclass = new Timeclass;
  timeclass->run();
  connect(timeclass->timer, SIGNAL(timeout()), this, SLOT(smr_timersignal()));

  
  visualizer = new Visual(smbtawidget);
//  visualizer->visuallabel->setText("Jetzt aber.");
  visualizer->visualwidget->show();
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
  
  qDebug() << "++++++++++++++++++++++++++++++++++++++++";
  qDebug()<<"smr_timersignal";
//  *visualizer->l_visualread  = *l_readstack;
//  *visualizer->l_visualwrite = *l_writestack;
  qDebug() << "*l_readstack  : " << *l_readstack;
  qDebug() << "*l_writestack  : " << *l_writestack;  
//  qDebug() << "*visualizer->l_visualread: " << *visualizer->l_visualread;
//  qDebug();
  visualizer->vs_processnumbers(l_readstack, l_writestack);

  *l_readstack  =  0;
  *l_writestack = 0;

//  sleep(1);
  
//  qDebug() << "*l_readstack 2: " << *l_readstack;
//  qDebug() << "*visualizer->l_visualread 2 : " << *visualizer->l_visualread;
//  qDebug();
  
  
//  qDebug() << "*l_readstack 3: " << *l_readstack;
//  qDebug() << "*visualizer->l_visualread 3 : " << *visualizer->l_visualread;
  qDebug() << "---------------------------------------------------";
  qDebug(); qDebug();
  
  mutex.unlock();
}

void Smbtamonitor_run::smr_visualizer(){
   
  qDebug()<<"smr_visualizer"; 
  
}

void Smbtamonitor_run::smr_sendmessage(){
//      qDebug()<< "Smbtamonitor_run  4";
//  i_debug++;
//  qDebug()<<i_debug;
//  if(i_debug == 1000000){
//   processrunner->monitorprocess->close();
//   qDebug() << "fin";
//   i_debug=0;
    
//  }
 
  output = new QString;
  *output = processrunner->monitorprocess->readLine();
  smr_parsemonitor();
//  qDebug() << *output;
     
  
}

void Smbtamonitor_run::smr_parsemonitor(){
  
  if( output->startsWith("W")){
//    qDebug() << "Write" << *output;
    output->replace("W: ","");
//    qDebug()<< "Replaced .." << *output; 
     *l_writestack += output-> toULong();
//    qDebug() << output->toInt();
  }
  
  if( output->startsWith("R")){
    output->replace("R: ","");
//    qDebug()<< "Replaced .." << *output; 
     *l_readstack += output->toULong();
//    qDebug() << i_out;
  }
  
  runtestline->setText(*output);
  
}

#include "smbtamonitor-run.moc"
