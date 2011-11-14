 
 # include "visual.h"


  
 Visual::~Visual(){}

 Visual::Visual(QWidget *parent)
{
  qDebug()<< "Class Visual";

  l_visualread = new unsigned long;
  l_visualwrite = new unsigned long;
  l_currentmax = new unsigned long; l_historymax = new unsigned long;
  *l_currentmax = 0;
  
  
  
  visualwidget = new QWidget(this);
  visuallayout = new QVBoxLayout(visualwidget);
  visuallabel = new QLabel("Visuallabel",visualwidget);
  visualreadvalue = new QLabel("Visualreadvalue: ", visualwidget);
  visualwritevalue = new QLabel("Visualwritevalue: ", visualwidget);
  visualhistorymax = new QLabel("Visuahistorymax: ", visualwidget);


  
  visuallayout->addWidget(visuallabel,0);
  visuallayout->addWidget(visualreadvalue,1);
  visuallayout->addWidget(visualwritevalue,2);
  visuallayout->addWidget(visualhistorymax,3);

  visualwidget->setLayout(visuallayout);

  
//  painter = new QPainter;
} 



void Visual::vs_processnumbers(long *l_read, long *l_write){
  
//  qDebug() << " l_visualread: "  << *l_visualread;
//  qDebug() << " l_visualwrite: " << *l_visualwrite;

  *l_currentmax = *l_read + *l_write;
  qDebug() << *l_historymax;
  if(*l_currentmax > *l_historymax){*l_historymax = *l_currentmax; visualhistorymax->setText(QString::number(*l_currentmax));}
  qDebug() << " *l_read: "  << *l_read;
  qDebug() << " *l_write: " << *l_write;
  
  
}

void Visual::vs_painter(){
  
  
  
}




#include "visual.moc"
