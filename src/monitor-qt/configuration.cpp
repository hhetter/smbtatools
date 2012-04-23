

#include "configuration.h"


Configuration::~Configuration()
{}

//Configuration::Configuration(QWidget *parent)
Configuration::Configuration()
{

   qDebug() << "Constructor Configuration class";
   remoteconfigbutton = new QPushButton("remotething");
//    configwidget = new QWidget(this);
//    configgridlayout = new QGridLayout(configwidget);
//    message1 = new QLabel("Eins", configwidget);
//    message2 = new QLabel("zweo", configwidget);



   hostline = new QLineEdit("smbtad.de");
   portline = new QLineEdit("3491");
//    shareline = new QLineEdit("holger");
   sharebox = new QComboBox();
   timeline = new QLineEdit("10m");
   timeframe = new QLineEdit("60s");

//    configform = new QFormLayout(configwidget);

   // Add entries to sharebox
   sharebox->addItem("holger");
   sharebox->addItem("something else");


   configform = new QFormLayout(this);
   configform->addRow("Hostname:", hostline);
   configform->addRow("Port:", portline);
//    configform->addRow("Share:", shareline);
   configform->addRow("Share:", sharebox);
   configform->addRow("Monitor time:", timeline);
   configform->addRow("Time frame:", timeframe);
//    configwidget->show();
//    this->show();
//    configgridlayout->addWidget(message1, 1,1);
//    configgridlayout->addWidget(message2, 10,1);

}

#include "configuration.moc"




/*

#include "configuration.h"

Configuration::Configuration()
{


    qDebug()<< "This is the Configurator";

}


Configuration::~Configuration()
{
}
*/
