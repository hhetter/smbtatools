 
 # include "visual.h"
#include <talloc.h>

  
 Visual::~Visual(){}

 Visual::Visual(QWidget *parent) : QWidget(parent)
{
  qDebug()<< "Class Visual";

  l_visualread = new unsigned long;
  l_visualwrite = new unsigned long;
  l_currentmax = new unsigned long; l_historymax = new unsigned long;
  *l_currentmax = 0; *l_historymax = 0;
  i_time = 0; i_oldscalefactor = 1; i_scalefactor = 1;
  
  visualwidget = new QWidget(this);
  visuallayout = new QVBoxLayout(visualwidget);
  visuallabel = new QLabel("Visuallabel",visualwidget);
//  visualreadvalue = new QLabel("Visualreadvalue: ", visualwidget);
//  visualwritevalue = new QLabel("Visualwritevalue: ", visualwidget);
  visualhistorymax = new QLabel("Visuahistorymax: ", visualwidget);
  
//  visuallayout->addWidget(visuallabel,0);
//  visuallayout->addWidget(visualreadvalue,1);
//  visuallayout->addWidget(visualwritevalue,2);
  visuallayout->addWidget(visualhistorymax,3);

  visualwidget->setLayout(visuallayout);
   xstring1 = QString(); xstring2 = QString(); xstring3 = QString(); xstring4 = QString(); xstring5 = QString();
 // xstring1.append("0 kb"); xstring2.append("25 kb"); xstring3.append("50 kb"); xstring4.append("100 kb");xstring5.append("125 kb");

} 

char *Visual::mhr( TALLOC_CTX *ctx, long long int kb )
{
	char kbstring[20];
	char *output;
	long long int result = kb;
        long long int rest = 0;
        lldiv_t diff;
        strcpy(kbstring,"Bytes");
        if (kb >= (long long )1024*1024*1024*1024) {
                diff = lldiv(kb,(long long ) 1024*1024*1024*1024); // tb
                strcpy(kbstring,"TB");
                result = diff.quot;
                rest = diff.rem;
	        } else
																	        if (kb >= (long long )1024*1024*1024) {
                diff = lldiv(kb,(long long) 1024*1024*1024); // gb
                strcpy(kbstring,"GB");
                result = diff.quot;
                rest = diff.rem;
		} else
																	        if (kb >= (long long) 1024*1024) {
               diff = lldiv(kb,(long long) 1024*1024); // mb
               strcpy(kbstring,"MB");
               result = diff.quot;
               rest = diff.rem;
		} else
     if (kb >= 1024) {
               diff =  lldiv(kb, (long long) 1024); // kb
               strcpy(kbstring,"KB");
               result = diff.quot;
               rest = diff.rem;
																		        }
     output = talloc_asprintf( ctx,"%lli.%lli %s",result,rest,kbstring);
     return output;
}


void Visual::vs_processnumbers(unsigned long *l_read, unsigned long *l_write){
  
//  qDebug() << " l_visualread: "  << *l_visualread;
//  qDebug() << " l_visualwrite: " << *l_visualwrite;
TALLOC_CTX *ctx = NULL;


  // Reset Graph painterpaths
  readpath  = QPainterPath();
  writepath = QPainterPath();



  *l_currentmax = (*l_read + *l_write);

  if(*l_currentmax > *l_historymax){
    
    *l_historymax = *l_currentmax; visualhistorymax->setText(QString( mhr(ctx, (long long int) *l_currentmax)));
  
    ////
    // Traffic values need to be scaled to the graph range
    // y-Axis ranges 400 px, so historymax must be displayed with a shorter range
    // If scale changes, all values need to be recalculated
    //
     if(i_scalefactor != 1){i_oldscalefactor = i_scalefactor;}
    i_scalefactor = (1.1)*(*l_historymax)/400;
//    qDebug() << "i_scalefactor: " << i_scalefactor;
    
    // Rescale all values in the current vectors
    if(i_oldscalefactor != 1){
      for(int i=0; i < i_time; i++){	
	writev[i].setY( writev[i].y() * (i_scalefactor/i_oldscalefactor) );
	readv[i].setY( readv[i].y() * (i_scalefactor/i_oldscalefactor) );
      }
    }
    
    ////
    // Rescale axes
    xstring5 = QString( mhr( ctx,(long long) (1.1*(*l_historymax))));
    xstring4 = QString( mhr( ctx,(long long) (0.75*1.1*(*l_historymax))));;
    xstring3 = QString( mhr( ctx,(long long) (0.5*1.1*(*l_historymax))));;
    xstring2 = QString( mhr( ctx,(long long) (0.25*1.1*(*l_historymax))));;
    xstring1 = QString( mhr( ctx,(long long) (0*1.1*(*l_historymax))));;
//    qDebug() << mhr(ctx,(long long) (0*1.1*(*l_historymax)));
    talloc_free(ctx);
  
  }
  
    // debugging messages
//  qDebug() <<"*l_historymax: " << *l_historymax;
//  qDebug() << " *l_read: "  << *l_read;
//  qDebug() << " *l_write: " << *l_write;
   
  
  // Create QPoints according to the r/w input and scaling
  readp.setY(500 - (*l_read/i_scalefactor));readp.setX(650-i_time);
  writep.setY(500 -( ((*l_write+*l_read) /i_scalefactor) ));writep.setX(650-i_time);

  
  // Create QVector<QPoint>'s with the scanned graph datasets
  readv<<(readp);
  writev<<(writep);
//  qDebug() << "writev.size() " << writev.size();
//  qDebug() << "i_time: " << i_time;
  
  ////
  // Create QPaths from the QVector<QPoint>'s
  // Inverse order of the vectors is needed so that every new value gets added to the 
  // right end of the graph,the rest gets just shifted one point to the left
  writepath.moveTo(650-i_time, writev[0].y());
  readpath.moveTo( 650-i_time, readv[0].y());

  for(int i = i_time; i >= 0; i--){
    writepath.lineTo(650-i, writev[i_time-i].y());
    readpath.lineTo(650-i, readv[i_time-i].y());    
  }

  if(i_time < 600){
    i_time++;
  }

  // call the qpainter
  update();
  
  
}

void Visual::paintEvent(QPaintEvent *){

  QPainter painter(this);
  QRect graphbr(50,100,600,400);
  QRect writerect(50,510,10,10);
  QRect readrect(50, 530, 10, 10);
  QPen writepen(Qt::blue, 1);
  QPen readpen(Qt::red, 1);

  // Create Graph Area
  painter.drawRect(graphbr);
  painter.fillRect(graphbr, QColor(255,255,255));
  painter.drawLine(50, 200, 650, 200);
  painter.drawLine(50, 300, 650, 300);
  painter.drawLine(50, 400, 650, 400);

  // Create Axis / Labeling
  painter.setPen(Qt::black);
  painter.setFont(QFont("Arial", 8));
  painter.drawText(0,500, xstring1);painter.drawText(0,400, xstring2);painter.drawText(0,300, xstring3);painter.drawText(0,200, xstring4);painter.drawText(0,100, xstring5);
  painter.drawText(65,520, "Write traffic");
  painter.drawText(65,540, "Read traffic");
  painter.setPen(writepen);
  painter.drawRect(writerect);painter.fillRect(writerect, Qt::blue);
  painter.setPen(readpen);
  painter.drawRect(readrect);painter.fillRect(readrect, Qt::red);
  
  // Paint graphs
  painter.setPen(readpen);
  painter.drawPath(readpath);
  painter.setPen(writepen);
  painter.drawPath(writepath);

}




#include "visual.moc"
