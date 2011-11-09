

 #include "frontend.h"
 #include "configuration.h"
 #include "smbtamonitor-run.h"

// Class Configuration;

// Frontend class destructor
 Frontend::~Frontend()
 {}

// Frontend class constructor
 Frontend::Frontend(QWidget *parent)
  
{

 i_debug=0;
  
  
      configurator = new Configuration;
 //      Configuration configclass(this);
 //     i_frontendvariable = 0;
 //     qDebug() << i_frontendvariable;
      qDebug("This is class constructor Frontend::Frontend");

      configbutton = new QPushButton("Configuration");
      connect(configbutton, SIGNAL(clicked()), this, SLOT(fr_config()));
      
      monitorbutton = new QPushButton("Monitor");
      connect(monitorbutton, SIGNAL(clicked()), this, SLOT(fr_getmonitor()));
      
      dosomethingbutton = new QPushButton("Do something");
      connect(dosomethingbutton, SIGNAL(clicked()), this, SLOT(fr_dosomething()));
      
      
      quitbutton = new QPushButton("Quit");
      connect(quitbutton, SIGNAL(clicked()), qApp, SLOT(quit()));
      
      frontendwidget = new QWidget;
      frontendlayout = new QGridLayout(frontendwidget);
      gridlayout = new QGridLayout;
      
      frontendlayout->setColumnStretch(0, 0);
      
      frontendlayout->addWidget(configbutton,1,0);
      frontendlayout->addWidget(monitorbutton,2,0);
      frontendlayout->addWidget(dosomethingbutton,3,0);
      frontendlayout->addWidget(quitbutton,4,0);
 //     gridlayout->addWidget(configurator->configwidget,1,1,3,3);
      outputline = new QLabel("QLabel outputline");
      frontendlayout->addWidget(outputline,5,0);

      gridlayout->addWidget(frontendwidget,0,0);
      setLayout(gridlayout);  

}

void Frontend::fr_sendmessage(){
  i_debug++;
  qDebug()<<i_debug;
  if(i_debug == 10000){
//   smbtamonitor_runner->monitorprocess->close();
//   qDebug() << "fin";
//   i_debug=0;
    
  }
  output = new QString;
//  *output = smbtamonitor_runner->monitorprocess->readLine();
  fr_parsemonitor();
//  qDebug() << *output;
     
  
}

void Frontend::fr_parsemonitor(){
  
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

void Frontend::fr_config(){
  
 qDebug()<< "fr_config"; 
  gridlayout->addWidget(configurator->configwidget,0,1,3,3);
  
}

void Frontend::fr_getmonitor(){

  smbtamonitor_runner = new Smbtamonitor_run;
  smbtamonitor_runner->run();
//  connect(smbtamonitor_runner->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(fr_sendmessage()));
  
  
  
  
  
}

void Frontend::fr_dosomething(){
  
  i_debug++;
  QString astring;
  
  astring.append(i_debug);
 
  outputline->setText(astring);
  
}



#include "frontend.moc"
