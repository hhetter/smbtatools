 
 # include "visual.h"


  
 Visual::~Visual(){}

 Visual::Visual(QWidget *parent) : QWidget(parent)
{
  qDebug()<< "Class Visual";

  l_visualread = new unsigned long;
  l_visualwrite = new unsigned long;
  l_currentmax = new unsigned long; l_historymax = new unsigned long;
  *l_currentmax = 0; *l_historymax = 0;
  int i_time = 0;
  
  
  
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
//  QPaintEvent blubb;
  xstring1.append("0 kb"); xstring2.append("25 kb"); xstring3.append("50 kb"); xstring4.append("100 kb");xstring5.append("125 kb");
//  update();
  
//  vs_painter(blubb);
} 



void Visual::vs_processnumbers(unsigned long *l_read, unsigned long *l_write){
  
//  qDebug() << " l_visualread: "  << *l_visualread;
//  qDebug() << " l_visualwrite: " << *l_visualwrite;

 

  *l_currentmax = (*l_read + *l_write);

  if(*l_currentmax > *l_historymax){*l_historymax = *l_currentmax; visualhistorymax->setText(QString::number(*l_currentmax));}
  qDebug() <<"*l_historymax: " << *l_historymax;
  qDebug() << " *l_read: "  << *l_read;
  qDebug() << " *l_write: " << *l_write;
  
  ////
  // Create QPoints according to the r/w input
  readp.setY(500 - (*l_read));readp.setX(650-i_time);
  writep.setY(500 -(*l_write));writep.setX(650-i_time);
  i_time++;
  
  ////
  // Append points to respective vectors
  readv<<readp;
  writev<<writep;
  ////
  // Create QPolygon to be filled into the graph later on
  
  
  
    
  update();
  
  
}

void Visual::paintEvent(QPaintEvent *){
  
  QPainter painter(this);
  QRect graphbr(50,100,600,400);
  QPen writepen(Qt::blue, 1);
  QPen readpen(Qt::red, 1);
  painter.drawRect(graphbr);
  painter.fillRect(graphbr, QColor(255,255,255));
  painter.drawLine(50, 200, 650, 200);
  painter.drawLine(50, 300, 650, 300);
  painter.drawLine(50, 400, 650, 400);

//  painter.drawLine(0, 300 , 600, 300);
  painter.setPen(Qt::black);

  painter.setFont(QFont("Arial", 8));
  painter.drawText(0,500, xstring1);painter.drawText(0,400, xstring2);painter.drawText(0,300, xstring3);painter.drawText(0,200, xstring4);painter.drawText(0,100, xstring5);
 // painter.drawRect(0,100,400,400);
  painter.setPen(readpen);
  painter.drawLines(readv);
  painter.setPen(writepen);
  painter.drawLines(writev);  
  
  
}




#include "visual.moc"
