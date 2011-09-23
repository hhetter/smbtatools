 
# include "configuration.h"


 Configuration::~Configuration()
 {}

 Configuration::Configuration(QWidget *parent)
{
  
    qDebug() << "bleh";
    remoteconfigbutton = new QPushButton("remotething");
    configwidget = new QWidget;
    configgridlayout = new QGridLayout(configwidget);
    message1 = new QLabel("Eins", configwidget);
    message2 = new QLabel("zweo", configwidget);
    configgridlayout->addWidget(message1, 1,1);
    configgridlayout->addWidget(message2, 10,1);

}