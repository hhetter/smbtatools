 

 # include "configuration.h"


 Configuration::~Configuration()
 {}

 Configuration::Configuration(QWidget *parent)
{
  
    qDebug() << "bleh";
    remoteconfigbutton = new QPushButton("remotething");
    configwidget = new QWidget;
//    configgridlayout = new QGridLayout(configwidget);
//    message1 = new QLabel("Eins", configwidget);
//    message2 = new QLabel("zweo", configwidget);
    
    hostline = new QLineEdit("localhost");
    portline = new QLineEdit("3942");
    shareline = new QLineEdit("holger");
    timeline = new QLineEdit("1h");
 
    configform = new QFormLayout(configwidget);
    configform->addRow("Hostname:", hostline);
    configform->addRow("Port:", portline);
    configform->addRow("Share:", shareline);
    configform->addRow("Timeframe:", timeline);
    
//    configgridlayout->addWidget(message1, 1,1);
//    configgridlayout->addWidget(message2, 10,1);

}

#include "configuration.moc"