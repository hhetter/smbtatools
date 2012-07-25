#include "graph.h"

Graph::Graph(QWidget *parent) :
        QWidget(parent)
{
        i_max_index = 86400;
        i_s_count = 0;
        i_stepsize=5;
        i_x_os = 50; // Offset for x Graph
        i_y_os = 0; // Offset for y Graph
        i_x_d_size = 600;
        i_y_d_size = 400;
        i_dp_min = 50;
        i_dp_max = 2500;
        i_dp_number = i_x_d_size;


        i_dp_start = 0; // index of the data point from where the display is started
        i_dp_end = i_dp_number + i_dp_start; // index of the data point from where the display  ends

        readlist = new QList<unsigned long>;
        writelist = new QList<unsigned long>;


        xstring1="1";
        xstring2="2";
        xstring3="3";
        xstring4="4";
        xstring5="5";
}



void Graph::g_receivelist(QList<unsigned long> readlist_in,
                          QList<unsigned long> writelist_in)
{
        //            qDebug()<<"g_receivelist.readlist.size: "<< readlist.size();

        //    readlist = &readlist_in;
        //   qDebug()<< readlist[0];
        //   qDebug()<<"end";
        //   writelist = &writelist_in;
        //g_get_w_size();

        g_interpolate(readlist_in, writelist_in);
        g_create_path();


}


/*
 * Get the size of the widget that contains the graph
 * Basic implementation done
 * Widget size doesn't change yet so no visible effect
 * Method not called yet
 */
void Graph::g_get_w_size()
{
        //QSize w_size = this->frameSize();
        /*    qDebug()<<"QSize: y " << w_size.height();
    qDebug()<<"QSize calc: y " << (int)(w_size.height()*0.8);
    qDebug()<<"QSize: x " << w_size.width();
*/
}


// Get the datapoint offset to t=0
int Graph::g_get_dp_offset()
{

        return 0;
}



void Graph::g_change_dp_num(int i_delta) // Change the number of datapoints for the graph
{
        if(i_delta > 0 && i_dp_number > i_dp_min)
        {
                qDebug()<< "Zoom in";
                i_dp_number = (int)(i_dp_number*0.8);
        }

        if(i_delta < 0 && i_dp_number < i_dp_max)
        {
                qDebug()<< "Zoom out";
                i_dp_number = (int)(i_dp_number*1.2);
        }
        i_dp_start = i_dp_start + g_get_dp_offset();
        i_dp_end = i_dp_start + i_dp_number;
        qDebug() << "i_dp_start: " << i_dp_start;
        qDebug() << "i_dp_end: " << i_dp_end;
}


// Not needed with the current graph model
void Graph::g_def_dp_num() // Define how to map datapoints to pixels
{
        // 1) find out if there are more pixels or more datapoints
        if(i_dp_number <= i_dp_number){
                g_release_dp();
        }
        if(i_dp_number > i_dp_number){
                g_squeeze_dp();
        }
        // 2) find a method to map datapoints to pixels
        //-> see the two methods called from here, g_release_dp(), g_squeeze_dp();
}

/*
 * Distribute data points to the graph when the pixel width is greater than the
 * number of data points
 */
// Not needed with the current graph model
void Graph::g_release_dp()
{
}

/*
 * Distribute data points to the graph when the number of data points is greater
 * than the pixel width.
 */
// Not needed with the current graph model
void Graph::g_squeeze_dp()
{
}


// Interpolation and create points to make the graph
void Graph::g_interpolate(QList<unsigned long> readlist_in,
                          QList<unsigned long> writelist_in)
{
        // Initialize Values
        l_max  = 0;
        readp  = QPointF();
        writep = QPointF();
        readpg = QPolygonF();
        writepg = QPolygonF();
        l_read_diff = 0;
        l_write_diff = 0;


        for(int j = 0; j < 10; j++){
                //      qDebug() << "enterd value x "<<j<< ": " << readlist_in[j];
        }
        //    qDebug() << "================";

        // Find max value of read+write traffic to definy y-axis scale factor
        for(int i = i_dp_start; i < i_dp_end; i++){
                l_c_max = readlist_in[i] + writelist_in[i];
                if(l_c_max > l_max){
                        l_max = l_c_max;
                       // qDebug()<< "lmax-event detected: l_max, l_c_max" << l_max << " " << l_c_max;
                }
        }
      //  qDebug() << "++++++++++++++++++++";
      //  qDebug() << "l_max: " << l_max;
      //  qDebug() << "++++++++++++++++++++";
        f_scalefactor = 1.1*((float)l_max)/i_y_d_size;

        // Interpolate graph into i_stepsize subpoints
        for(int i = i_dp_start; i < (i_dp_end*i_stepsize); i++)
        {
                // Do interpolation later
                /*
                if(i_intpol_count < i_stepsize )
                {
                        *l_in_read += *l_fread;
                        *l_in_write += *l_fwrite;
                        i_c_count++;
                }
                if (i_intpol_count == i_stepsize)
                {

                        *l_in_read_end = *l_in_read; *l_in_read = 0;
                        *l_in_write_end = *l_in_write; *l_in_write = 0;
                        *l_read_diff = (*l_in_read_end)/i_stepsize;
                        *l_write_diff = (*l_in_write_end)/i_stepsize;
                        vs_processnumbers(
                                                ((*l_read_diff)*
                                                 ((unsigned long)(i_intpol_count%i_stepsize)+1)),
                                                ((*l_write_diff)*
                                                 ((unsigned long)(i_intpol_count%i_stepsize)+1))
                                                );
                        i_intpol_count++;
                }
                */
        }

        // Create Points
        // Create QPolygonF from right to left
        for(int i = i_dp_start; i < i_dp_end /* (i_dp_end*i_stepsize) */; i++)
        {
                readpg<<QPointF( (i_x_os + i_x_d_size - i),
                                 (i_y_os + i_y_d_size) -
                                 ((readlist_in[i])/f_scalefactor));
                writepg<<QPointF( (i_x_os + i_x_d_size - i),
                                  (i_y_os + i_y_d_size) -
                                  (((writelist_in[i])/f_scalefactor) +
                                   ((readlist_in[i])/f_scalefactor)
                                   )
                                  );
        }
        // Create QPolygonF back from left to right
        for(int i = i_dp_end; i <= i_dp_start /* (i_dp_end*i_stepsize) */; i--)
        {
                readpg<<QPointF( (i_x_os + i_x_d_size - i),
                                 (i_y_os + i_y_d_size) -
                                 0);
                writepg<<QPointF( (i_x_os + i_x_d_size - i),
                                  (i_y_os + i_y_d_size) -
                                  (0                                                                     )
                                  );
        }
        readpg<<QPointF( (i_x_os + i_x_d_size ),
                         (i_y_os + i_y_d_size) -
                         ((readlist_in[i_dp_start])/f_scalefactor));
        writepg<<QPointF( (i_x_os + i_x_d_size ),
                          (i_y_os + i_y_d_size) -
                          (((writelist_in[i_dp_start])/f_scalefactor) +
                           ((readlist_in[i_dp_start])/f_scalefactor)
                           )
                          );

        update();


}


void Graph::g_create_path()
{




}


void Graph::paintEvent(QPaintEvent *){


        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.scale(1.0,1.0);
        QRect graphbr(i_x_os,i_y_os,i_x_d_size,i_y_d_size);
        QRect writerect(50,i_y_os+i_y_d_size+10,10,10);
        QRect readrect(50, i_y_os+i_y_d_size+30, 10, 10);
        QPen writepen(Qt::blue, 1);
        QPen readpen(Qt::red, 1);


        //  QPolygon testpolygon; testpolygon<<QPoint(200,200)<<QPoint(400,200)<<QPoint(400,400)<<QPoint(200,400);

        // Create Graph Area
        painter.drawRect(graphbr);
        painter.fillRect(graphbr, QColor(255,255,255));


        // Create Axis / Labeling
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 8));
        painter.scale(1.0,1.0);

        painter.drawText(0,i_y_os+i_y_d_size+10, xstring1);
        painter.drawText(0,i_y_os+10+(i_y_d_size*0.75), xstring2);
        painter.drawText(0,i_y_os+10+(i_y_d_size*0.50), xstring3);
        painter.drawText(0,i_y_os+10+(i_y_d_size*0.25), xstring4);
        painter.drawText(0,i_y_os+10, xstring5);

        painter.drawText(65,i_y_os+i_y_d_size+20, "Write traffic");
        painter.drawText(65,i_y_os+i_y_d_size+40, "Read traffic");
        painter.scale(1.0,1.0);
        painter.setPen(writepen);
        painter.drawRect(writerect);painter.fillRect(writerect, Qt::blue);
        painter.setPen(readpen);
        painter.drawRect(readrect);painter.fillRect(readrect, Qt::red);

        // Paint graphs
        painter.scale(1.5,1.0);
        painter.setPen(writepen);
        //  painter.drawPath(writepath);
        painter.setBrush(QBrush(Qt::blue, Qt::SolidPattern));
        painter.drawPolygon(writepg);
        painter.setPen(readpen);
        //  painter.drawPath(readpath);
        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        painter.drawPolygon(readpg);
        painter.scale(1.0,1.0);

        painter.setPen(Qt::black);
        painter.drawLine(i_x_os, i_y_os,                i_x_os+i_x_d_size, i_y_os);
        painter.drawLine(i_x_os, i_y_os+(i_y_d_size*0.25), i_x_os+i_x_d_size, i_y_os+(i_y_d_size*0.25));
        painter.drawLine(i_x_os, i_y_os+(i_y_d_size*0.50), i_x_os+i_x_d_size, i_y_os+(i_y_d_size*0.50));
        painter.drawLine(i_x_os, i_y_os+(i_y_d_size*0.75), i_x_os+i_x_d_size, i_y_os+(i_y_d_size*0.75));
        painter.drawLine(i_x_os, i_y_os+(i_y_d_size),      i_x_os+i_x_d_size, i_y_os+i_y_d_size);
        painter.drawLine(i_x_os, i_y_os,                i_x_os,         i_y_os+i_y_d_size); // draws left border of the rect black


}







void Graph::wheelEvent(QWheelEvent *event)
{
       // qDebug() << "Wheel event detected";
       int y =  event->delta();
        g_change_dp_num(y);
}



#include "graph.moc"
