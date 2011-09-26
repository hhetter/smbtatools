

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
  
      configurator = new Configuration;
 //      Configuration configclass(this);
 //     i_frontendvariable = 0;
 //     qDebug() << i_frontendvariable;
      qDebug("This is class constructor Frontend::Frontend");

      configbutton = new QPushButton("Configuration");
      connect(configbutton, SIGNAL(clicked()), this, SLOT(fr_config()));
      
      monitorbutton = new QPushButton("Monitor");
      connect(monitorbutton, SIGNAL(clicked()), this, SLOT(fr_getmonitor()));
      
      quitbutton = new QPushButton("Quit");
      connect(quitbutton, SIGNAL(clicked()), qApp, SLOT(quit()));
      
      frontendwidget = new QWidget;
      frontendlayout = new QGridLayout(frontendwidget);
      gridlayout = new QGridLayout;
      
      frontendlayout->setColumnStretch(0, 0);
      
      frontendlayout->addWidget(configbutton,1,0);
      frontendlayout->addWidget(monitorbutton,2,0);
      frontendlayout->addWidget(quitbutton,3,0);
 //     gridlayout->addWidget(configurator->configwidget,1,1,3,3);

      gridlayout->addWidget(frontendwidget,0,0);
      setLayout(gridlayout);  
}

void Frontend::fr_config(){
  
 qDebug()<< "give config"; 
  gridlayout->addWidget(configurator->configwidget,0,1,3,3);
  
}

void Frontend::fr_getmonitor(){
  
//  connect(smbtamonitor_runner->monitorprocess, SIGNAL(readyReadStandardOutput()), smbtamonitor_runner,SLOT(sr_emit()));
  smbtamonitor_runner = new Smbtamonitor_run;
 //  connect(smbtamonitur_runner->monitorprocess, SIGNAL(readyReadStandardOutput()), qApp,SLOT(quit()));
//  connect(smbtamonitor_runner->monitorprocess, SIGNAL(readyReadStandardOutput()), smbtamonitor_runner,SLOT(sr_emit()));
//   connect(smbtamonitor_runner->monitorprocess, SIGNAL(readyReadStandardOutput()), this,SLOT(fr_echo()));
  smbtamonitor_runner->run();
  
  
  
  
  
}

void Frontend::fr_echo(){
  qDebug()<<"echo";
  
  
}
  
  