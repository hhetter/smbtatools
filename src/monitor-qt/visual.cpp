
# include "visual.h"
#include <talloc.h>


Visual::~Visual(){}

Visual::Visual( QWidget *parent, int i_timeframe) : QWidget(parent)
{
 qDebug()<< "Class Visual";

 l_visualread = new unsigned long;
 l_visualwrite = new unsigned long;
 l_currentmax = new unsigned long; l_historymax = new unsigned long;

 l_in_read = new unsigned long; *l_in_read = 0;
 l_in_write = new unsigned long; *l_in_write = 0;
 l_in_read_start = new unsigned long; *l_in_read_start = 0;
 l_in_read_end = new unsigned long; *l_in_read_end = 0;
 l_in_write_start = new unsigned long; *l_in_write_start = 0;
 l_in_write_end = new unsigned long; *l_in_write_end = 0;
 l_read_diff = new unsigned long;
 l_write_diff = new unsigned long;

 *l_currentmax = 0; *l_historymax = 0;
 l_readmax = 0; l_writemax = 0;
 i_time = 0; f_oldscalefactor = 1; f_scalefactor = 1;
 i_x_os = 50;  i_y_os = 50; // Offset for x- and y-Graph
 i_x_max = 600; i_y_max = 400;   // Range of x- and y-Graph
 i_x = 0;
 i_c_count = 0;

 ////
 //
 // Calculate the stepping
 i_step = i_x_max/i_timeframe;

 visualwidget = new QWidget(this);
 visuallayout = new QVBoxLayout(visualwidget);
//  visuallabel = new QLabel("Visuallabel",visualwidget);
//  visualreadvalue = new QLabel("Visualreadvalue: ", visualwidget);
//  visualwritevalue = new QLabel("Visualwritevalue: ", visualwidget);
//  visualhistorymax = new QLabel("Visuahistorymax: ", visualwidget);

//  visuallayout->addWidget(visuallabel,0);
//  visuallayout->addWidget(visualreadvalue,1);
//  visuallayout->addWidget(visualwritevalue,2);
//  visuallayout->addWidget(visualhistorymax,3);

 visualwidget->setLayout(visuallayout);
//   xstring1 = QString(); xstring2 = QString(); xstring3 = QString(); xstring4 = QString(); xstring5 = QString();
 xstring1 ="0 kb"; xstring2 ="25 kb"; xstring3 = "50 kb"; xstring4 = "100 kb";xstring5 = "125 kb";

}

char *Visual::mhr( long long int kb )
{
    char kbstring[20];
    char *output;
    long long int result = kb;
//    long long int rest = 0;
    lldiv_t diff;
    strcpy(kbstring,"Bytes");
    if (kb >= (long long )1024*1024*1024*1024) {
        diff = lldiv(kb,(long long ) 1024*1024*1024*1024); // tb
        strcpy(kbstring,"TB");
        result = diff.quot;
//        rest = diff.rem;
    } else
        if (kb >= (long long )1024*1024*1024) {
            diff = lldiv(kb,(long long) 1024*1024*1024); // gb
            strcpy(kbstring,"GB");
            result = diff.quot;
//            rest = diff.rem;
        } else
            if (kb >= (long long) 1024*1024) {
                diff = lldiv(kb,(long long) 1024*1024); // mb
                strcpy(kbstring,"MB");
                result = diff.quot;
//                rest = diff.rem;
            } else
                if (kb >= 1024) {
                    diff =  lldiv(kb, (long long) 1024); // kb
                    strcpy(kbstring,"KB");
                    result = diff.quot;
//                    rest = diff.rem;
                }
    //     asprintf( &output,"%lli.%lli %s",result,rest,kbstring);
    asprintf( &output,"%lli %s",result,kbstring);
    return output;
}

void Visual::vs_wraptraffic(unsigned long *l_read, unsigned long *l_write, int i_timeframe){



}

void Visual::vs_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite){


    if(i_c_count < 4 )
    {
        *l_in_read += *l_fread;
        *l_in_write += *l_fwrite;
        i_c_count++;
    }
    if (i_c_count == 4)
    {

        *l_in_read_end = *l_in_read; *l_in_read = 0;
        *l_in_write_end = *l_in_write; *l_in_write = 0;
        *l_read_diff = (*l_in_read_end)/5;
        *l_write_diff = (*l_in_write_end)/5;
        vs_processnumbers( ((*l_read_diff)*((unsigned long)(i_c_count%5)+1)), ((*l_write_diff)*((unsigned long)(i_c_count%5)+1)) );
        i_c_count++;
    }
    if (i_c_count > 4 && i_c_count %5 != 0)
    {

        *l_in_read += *l_fread;
        *l_in_write += *l_fwrite;
        vs_processnumbers( (*l_read_diff*((i_c_count%5)+1)), (*l_write_diff*((i_c_count%5)+1)) );
        i_c_count++;
    }
    if (i_c_count > 4 && i_c_count %5 == 0)
    {
        *l_in_read_end = *l_in_read; *l_in_read = 0;
        *l_in_write_end = *l_in_write; *l_in_write = 0;
        *l_read_diff = *l_in_read_end/5;
        *l_write_diff = *l_in_write_end/5;
        vs_processnumbers( (*l_read_diff*((i_c_count%5)+1)), (*l_write_diff*((i_c_count%5)+1)) );

        i_c_count++;
    }

}


void Visual::vs_processnumbers(unsigned long l_read, unsigned long l_write){

//    qDebug()<<"vs_processnumbers";

//    qDebug() << "l_read : " << l_read;
//    qDebug() << "l_write: " << l_write;
//    qDebug()<<"...";


 ////
 //
 // Create nice bars

 if(l_read > l_readmax){

   l_readmax = l_read;

 }

 if(l_write > l_writemax){

   l_writemax = l_write;

 }



 ////
 //
 // Create points array from the data
 readp.setY( l_read);readp.setX(i_x);
 writep.setY(( ((l_write+l_read) )));writep.setX(i_x);


 ////
 //
 // Compute max values, scaling

  *l_currentmax = (l_read + l_write);

 if( (l_read + l_write) > *l_historymax){

   *l_historymax = (l_read + l_write);// visualhistorymax->setText(QString()); // ( mhr(ctx, (long long int) *l_currentmax)));
   i_rescaletimer=0;
   f_scalefactor = (1.1)*((float)*l_historymax)/i_y_max;
//    qDebug() << "f_scalefactor: " << f_scalefactor;

   ////
   // (Re)scale axes

   char *dummy;
   dummy=mhr((long long) (1.1*(*l_historymax)));
   xstring5 = QString(dummy);// ( mhr( ctx,(long long) (1.1*(*l_historymax))));
   free(dummy);
   dummy=mhr((long long) (0.75*1.1*(*l_historymax)));
   xstring4 = QString(dummy);// ( mhr( ctx,(long long) (0.75*1.1*(*l_historymax))));;
   free(dummy);
   dummy=mhr((long long) (0.5*1.1*(*l_historymax)));
   xstring3 = QString(dummy);// ( mhr( ctx,(long long) (0.5*1.1*(*l_historymax))));;
   free(dummy);
   dummy=mhr((long long) (0.25*1.1*(*l_historymax)));
   xstring2 = QString(dummy);// ( mhr( ctx,(long long) (0.25*1.1*(*l_historymax))));;
   free(dummy);
   dummy=mhr((long long) (0*1.1*(*l_historymax)));
   xstring1 = QString(dummy);// ( mhr( ctx,(long long) (0*1.1*(*l_historymax))));;
   free(dummy);

 }

   // Find a new *l_historymax, if the old one moved out of the graph
   // not implemented yet
 if(i_rescaletimer == 600){
   f_switch = 0;
   for(int i =0; i < 600; i++){
//      if(f_switch < (   )  ){}

   }
 }
 ////
 ////
 // QPolygon Method
 ////
 // Create QPolygon from the QVector<QPoint>'s
 // Inverse order of the vectors is needed so that every new value gets added to the
 // right end of the graph,the rest gets just shifted one point to the left


 // Reset Graph painterpaths
 readpath  = QPainterPath();
 writepath = QPainterPath();

 writepg = QPolygonF();
 readpg =  QPolygonF();

 // Create QVector<QPoint>'s with the scanned graph datasets
 // New points are added to the beginning
 readv.prepend(readp);
 writev.prepend(writep);


// If the QVector has more than 600 entries, remove the last one
 if(i_time == 600){
     readv.remove(600);
     writev.remove(600);
 }





// Start of the Graph with its newest value on its right side
// qDebug()<<"i_time: " << i_time << "  1";
 writepg<<QPointF((i_x_os + i_time), (i_y_os + i_y_max) - (writev[0].y()/f_scalefactor));
 readpg<<QPointF( (i_x_os + i_time), (i_y_os + i_y_max) - (readv[0].y()/f_scalefactor));

//  qDebug() << "writev[0].y(): " << writev[0].y();
//  qDebug() << "i_y_os - (writev[0].y()/f_scalefactor): " << i_y_os - (writev[0].y()/f_scalefactor);


  for(int i = 1; i < i_time; i++){
    writepg<<QPointF((i_x_os + i_time)-i,  (i_y_os + i_y_max) - (writev[i].y()/f_scalefactor));
    readpg<< QPointF((i_x_os + i_time)-i,  (i_y_os + i_y_max) - (readv[i].y()/f_scalefactor));
   }
//qDebug()<<"readv[i_time-1] " << readv[i_time-1];
//qDebug()<<"readv[i_time] " << readv[i_time];
//  writepg<<QPointF((i_x_os),  (i_y_os + i_y_max) - (readv[i_time].y()/f_scalefactor));
//  readpg<<QPointF( (i_x_os),  450);


 for(int i = i_time-1; i > 0; i--){

   writepg<<QPointF((i_x_os + i_time)-i,  (i_y_os + i_y_max) - (readv[i].y()/f_scalefactor));
   readpg<<QPointF( (i_x_os + i_time)-i,   450);
 };


 if(i_time < 600){
   i_time++;
 }
 i_rescaletimer++;
 i_x++;





////
//  Old, working version. commented out to work on the new moving graph



/*
 ////
 ////
 // QPolygon Method
 ////
 // Create QPolygon from the QVector<QPoint>'s
 // Inverse order of the vectors is needed so that every new value gets added to the
 // right end of the graph,the rest gets just shifted one point to the left


 // Reset Graph painterpaths
 readpath  = QPainterPath();
 writepath = QPainterPath();

 writepg = QPolygonF();
 readpg =  QPolygonF();

 // Create QVector<QPoint>'s with the scanned graph datasets
 readv << (readp);
 writev<< (writep);

 writepg<<QPointF((i_x_os + i_x_max)-i_time, (i_y_os + i_y_max) - (writev[0].y()/f_scalefactor));
 readpg<<QPointF( (i_x_os + i_x_max)-i_time, (i_y_os + i_y_max) - (readv[0].y()/f_scalefactor));

//  qDebug() << "writev[0].y(): " << writev[0].y();
//  qDebug() << "i_y_os - (writev[0].y()/f_scalefactor): " << i_y_os - (writev[0].y()/f_scalefactor);

  for(int i = i_time-1; i >= 0; i--){
    writepg<<QPointF((i_x_os + i_x_max)-i,  (i_y_os + i_y_max) - (writev[i_x-i].y()/f_scalefactor));
    readpg<< QPointF((i_x_os + i_x_max)-i,  (i_y_os + i_y_max) - (readv[i_x-i].y()/f_scalefactor));
   }

  writepg<<QPointF((i_x_os + i_x_max),  (i_y_os + i_y_max) - (readv[i_x].y()/f_scalefactor));
  readpg<<QPointF( (i_x_os + i_x_max),  450);

 for(int i = 1; i <= i_time; i++){
   writepg<<QPointF((i_x_os + i_x_max)-i,  (i_y_os + i_y_max) - (readv[i_x-i].y()/f_scalefactor));
   readpg<<QPointF( (i_x_os + i_x_max)-i,   450);
 }


 if(i_time < 600){
   i_time++;
 }
 i_rescaletimer++;
 i_x++;
*/
// update();


//*********************************************************************************
//*********************************************************************************
//*********************************************************************************
//*********************************************************************************

 update();


}

void Visual::paintEvent(QPaintEvent *){

 QPainter painter(this);
 QRect graphbr(i_x_os,i_y_os,i_x_max,i_y_max);
 QRect writerect(50,460,10,10);
 QRect readrect(50, 480, 10, 10);
 QPen writepen(Qt::blue, 1);
 QPen readpen(Qt::red, 1);


//  QPolygon testpolygon; testpolygon<<QPoint(200,200)<<QPoint(400,200)<<QPoint(400,400)<<QPoint(200,400);

 // Create Graph Area
 painter.drawRect(graphbr);
 painter.fillRect(graphbr, QColor(255,255,255));


 // Create Axis / Labeling
 painter.setPen(Qt::black);
 painter.setFont(QFont("Arial", 8));
 painter.drawText(0,i_y_os+i_y_max, xstring1);painter.drawText(0,i_y_os+(i_y_max*0.75), xstring2);painter.drawText(0,i_y_os+(i_y_max*0.50), xstring3);painter.drawText(0,i_y_os+(i_y_max*0.25), xstring4);painter.drawText(0,i_y_os, xstring5);
 painter.drawText(65,470, "Write traffic");
 painter.drawText(65,490, "Read traffic");
 painter.setPen(writepen);
 painter.drawRect(writerect);painter.fillRect(writerect, Qt::blue);
 painter.setPen(readpen);
 painter.drawRect(readrect);painter.fillRect(readrect, Qt::red);

 // Paint graphs
 painter.setPen(writepen);
//  painter.drawPath(writepath);
 painter.setBrush(QBrush(Qt::blue, Qt::SolidPattern));
 painter.drawPolygon(writepg);
 painter.setPen(readpen);
//  painter.drawPath(readpath);
 painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
 painter.drawPolygon(readpg);

 painter.setPen(Qt::black);
 painter.drawLine(i_x_os, i_y_os,                i_x_os+i_x_max, i_y_os);
 painter.drawLine(i_x_os, i_y_os+(i_y_max*0.25), i_x_os+i_x_max, i_y_os+(i_y_max*0.25));
 painter.drawLine(i_x_os, i_y_os+(i_y_max*0.50), i_x_os+i_x_max, i_y_os+(i_y_max*0.50));
 painter.drawLine(i_x_os, i_y_os+(i_y_max*0.75), i_x_os+i_x_max, i_y_os+(i_y_max*0.75));
 painter.drawLine(i_x_os, i_y_os+(i_y_max),      i_x_os+i_x_max, i_y_os+i_y_max);
 painter.drawLine(i_x_os, i_y_os,                i_x_os,         i_y_os+i_y_max); // draws left border of the rect black

}




#include "visual.moc"
