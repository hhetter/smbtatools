 
 # include "visual.h"


  
 Visual::~Visual(){}

 Visual::Visual(QWidget *parent)
{
  
  
  qDebug()<< "Class Visual";
  
  visuallabel = new QLabel("Visuallabel");
} 

#include "visual.moc"
