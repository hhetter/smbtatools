
#include "frontend.h"
#include "configuration.h"


 class Configuration;

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

      monitorbutton = new QPushButton("Monitor");

      quitbutton = new QPushButton("Quit");
      connect(quitbutton, SIGNAL(clicked()), qApp, SLOT(quit()));
      
      gridlayout = new QGridLayout;
      
      gridlayout->setColumnStretch(0, 0);
      
      gridlayout->addWidget(configbutton,1,0);
      gridlayout->addWidget(monitorbutton,2,0);
      gridlayout->addWidget(quitbutton,3,0);
      gridlayout->addWidget(configurator->configwidget,1,1,3,1);
      setLayout(gridlayout);
  
}