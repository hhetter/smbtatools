 

 # include "configuration.h"


 Configuration::~Configuration()
 {}

 Configuration::Configuration(QWidget *parent)
{
  
    qDebug() << "Constructor Configuration class";
    remoteconfigbutton = new QPushButton("remotething");
    configwidget = new QWidget;
//    configgridlayout = new QGridLayout(configwidget);
//    message1 = new QLabel("Eins", configwidget);
//    message2 = new QLabel("zweo", configwidget);
    
    hostline = new QLineEdit("localhost");
    portline = new QLineEdit("3942");
    shareline = new QLineEdit("holger");
    timeline = new QLineEdit("10m");
    timeframe = new QLineEdit("60s");
 
    configform = new QFormLayout(configwidget);
    configform->addRow("Hostname:", hostline);
    configform->addRow("Port:", portline);
    configform->addRow("Share:", shareline);
    configform->addRow("Monitor time:", timeline);
    configform->addRow("Time frame:", timeframe);
    
//    configgridlayout->addWidget(message1, 1,1);
//    configgridlayout->addWidget(message2, 10,1);

}

#include "configuration.moc"