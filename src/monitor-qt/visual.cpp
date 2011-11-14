 
 # include "visual.h"


  
 Visual::~Visual(){}

 Visual::Visual(QWidget *parent)
{
  
  
  qDebug()<< "Class Visual";

  visualwidget = new QWidget(this);
  visuallabel = new QLabel("Visuallabel",visualwidget);
  visualreadvalue = new QLabel("Visualreadvalue: ", visualwidget);
  visualwritevalue = new QLabel("Visualwritevalue: ", visualwidget);
//  visuallabel->show();  
  
//  visualwidget->show();
      
//      visuallayout = new QGridLayout(visualwidget);

      
//      visuallayout->addWidget(visuallabel,1,0);
//      setLayout(visuallayout);
     
//      visualwidget->show();    
} 

#include "visual.moc"
