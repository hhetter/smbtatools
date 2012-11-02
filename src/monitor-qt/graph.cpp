#include "graph.h"




Graph::Graph(InstanceData *idata, QWidget *parent) :
        QWidget(parent)
{
        ldata = idata;

        this->resize(692,492);


        i_get_width = this->width();
        i_get_height = this->height();

        qDebug()<< "first Own height: " << this->height();


        i_max_index = 86400;
        i_s_count = 0;
        i_stepsize=ldata->i_stepsize;
        i_x_os = 50; // Offset for x Graph
        i_y_os = 0; // Offset for y Graph
        //        i_x_d_size = 600;
        i_x_d_size = 600;
        i_y_d_size = 400;
        i_dp_min = 10;
        i_dp_max = 86400;
        //i_dp_number = i_x_d_size;
        i_dp_number = 300;
        f_scalefactor = 1;
        f_zoomfactor = ((float)i_x_d_size)/((float)i_dp_number);
        thrputw = 0;
        thrputr = 0;

        i_dp_start = 0; // index of the data point from where the display is started
        i_dp_end = i_dp_number + i_dp_start; // index of the data point from where the display  ends

        readlist = new QList<unsigned long>;
        writelist = new QList<unsigned long>;

        /*
         * Initialize xstrings for first use, will be overwritten after first
         *      few datasets
        */
        xstring1 = "0 kb";
        xstring2 = "25 kb";
        xstring3 = "50 kb";
        xstring4 = "100 kb";
        xstring5 = "125 kb";

        title = "smbtamonitor-qt version ";
        title.append( SMBTAMONITOR_VERSION );


        // hold configoration data
        /*
        hostString   = new QString;
        fileString   = new QString;
        portString   = new QString;
        shareString  = new QString;
        userString   = new QString;
        domainString = new QString;

        *hostString   = "";
        *fileString = "";
        *portString   = "";
        *shareString  = "";
        *userString   = "";
        *domainString = "";
        */





        t_string = g_clock.currentTime().toString();
        t_i_string = QString::number(i_dp_number)+" Seconds";



}



void Graph::g_receivelist(QList<unsigned long> readlist_in,
                          QList<unsigned long> writelist_in)
{
        //          qDebug()<<"g_receivelist.readlist.size: "<< readlist.size();

        //    readlist = &readlist_in;
        //   qDebug()<< readlist[0];
        //   qDebug()<<"end";
        //   writelist = &writelist_in;
        //g_get_w_size();

        g_interpolate(readlist_in, writelist_in);
        //g_create_path();


}

QList<long long> Graph::g_prepare_data(QList<unsigned long> getlist)
{



        for(int i = i_dp_start; i < (i_dp_end*i_stepsize); i++)
        {
                worklist.append((long long)getlist[i]);
        }

        return worklist;



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
        int offset;
        int i_scalefactor = 4;
        if (i_dp_number < 20) { /* less than 20 datapoints we really zoom slowly */
                offset=i_scalefactor*1;
        } else if (i_dp_number < 60) { /* a minute */
                offset=i_scalefactor*2;
        } else if (i_dp_number < 300) { /* 5 minutes */
                offset=i_scalefactor*5;
        } else if (i_dp_number < 600) { /* 10 minutes */
                offset=i_scalefactor*10;
        } else if (i_dp_number < 1800) { /* 30 minutes */
                offset=i_scalefactor*50;
        } else if (i_dp_number < 5400) { /* 90 minutes */
                offset=i_scalefactor*200;
        } else offset=2*1200; /* anything else */


        if(i_delta > 0 && i_dp_number > i_dp_min)
        {
                // i_dp_number = (int)(i_dp_number*0.8);
                i_dp_number = i_dp_number-  offset;
        }

        if(i_delta < 0 && i_dp_number < i_dp_max)
        {

                //i_dp_number = (int)(i_dp_number*1.2);
                i_dp_number = i_dp_number+offset;
        }
        i_dp_start = i_dp_start + g_get_dp_offset();
        i_dp_end = i_dp_start + i_dp_number;
        f_zoomfactor = ((float)i_x_d_size)/((float)i_dp_number);
        t_i_string = QString::number(i_dp_number)+" Seconds";
        //        qDebug() << "i_dp_start: " << i_dp_start;
        //        qDebug() << "i_dp_end: " << i_dp_end;
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
        t_string = g_clock.currentTime().toString();


        // throughput per minute
        //
        thrputw = 0;
        thrputr = 0;
        for (int i = i_dp_start; i<i_dp_start+60; i++) {
                thrputw=thrputw+writelist_in[i];
                thrputr=thrputr+readlist_in[i];
        };



        // Find max value of read+write traffic to definy y-axis scale factor
        for(int i = i_dp_start; i < i_dp_end; i++){
                l_c_max = readlist_in[i] + writelist_in[i];
                if(l_c_max > l_max){
                        l_max = l_c_max;
                }
        }

        if(l_max > 1)
        {
                f_scalefactor = 1.1*((float)l_max)/i_y_d_size;
        }



        // Interpolate graph into i_stepsize subpoints
        // Probably not needed at all

        // Idea: For every dp there will be 5 graph-dps that will be used to build the graph
        //

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


        g_create_path(readlist_in, writelist_in);

}


void Graph::g_create_path(QList<unsigned long> readlist_in,
                          QList<unsigned long> writelist_in)
{

        // Create Points
        // Create QPolygonF from right to left
        // Uper side of the QPolygonF
        for(int i = i_dp_start; i < i_dp_end /* (i_dp_end*i_stepsize) */; i++)
        {
                readpg<<QPointF( ( i_x_d_size - i),
                                 ( i_y_d_size) -
                                 (((float)(readlist_in[i]))/f_scalefactor)
                                 );

                writepg<<QPointF( ( i_x_d_size - i),
                                  ( i_y_d_size) -
                                  ((((float)(writelist_in[i]))/f_scalefactor) +
                                   (((float)(readlist_in[i]))/f_scalefactor))
                                  );
        }


        readpg<<QPointF( ( i_x_d_size -i_dp_end),
                         ( i_y_d_size ) -
                         ((float)(readlist_in[i_dp_end]))/f_scalefactor
                         );
        //(((float)(readlist_in[i_dp_start]))/f_scalefactor));
        writepg<<QPointF( ( i_x_d_size - i_dp_end  ),
                          ( i_y_d_size) -
                          ((((float)(writelist_in[i_dp_end]))/f_scalefactor) +
                           (((float)(readlist_in[i_dp_end]))/f_scalefactor))
                          );








        // Create QPolygonF back from left to right
        for(int i = i_dp_end; i > i_dp_start /* (i_dp_end*i_stepsize) */; i--)
        {

                readpg<<QPointF( (i_x_d_size - i),
                                 ( i_y_d_size) -
                                 0
                                 );

                writepg<<QPointF( ( i_x_d_size - i),
                                  ( i_y_d_size) -
                                  (((float)(readlist_in[i]))/f_scalefactor)
                                  );
        }

        // Create end points of the QPolygonF's (at the right side)
        readpg<<QPointF( ( i_x_d_size ),
                         ( i_y_d_size) -
                         0);

        writepg<<QPointF( ( i_x_d_size ),
                          ( i_y_d_size) -
                          (((float)(readlist_in[i_dp_start]))/f_scalefactor)

                          );

        ////
        // (Re)scale axes
        if(l_max > 1024){
                xstring5 = QString(mhr((long long) (1.1*(l_max))));
                xstring4 = QString(mhr((long long) (0.75*1.1*(l_max))));
                xstring3 = QString(mhr((long long) (0.5*1.1*(l_max))));
                xstring2 = QString(mhr((long long) (0.25*1.1*(l_max))));
                xstring1 = QString(mhr((long long) (0*1.1*(l_max))));
        }
        /*
         * Call the paintEvent
         */
        update();




}


void Graph::paintEvent(QPaintEvent *){


        QPainter painter(this);

        //painter.scale(((float)(i_x_d_size))/((float)(i_dp_number)),1.0);
        painter.scale(1.0,1.0);
        painter.translate(i_x_os,i_y_os);
        QRect graphbr(0,0,i_x_d_size,i_y_d_size);
        QRect writerect(50+50,i_y_d_size+10,10,10);
        QRect readrect(135+50, i_y_d_size+10, 10, 10);
        QPen writepen(Qt::blue, 1);
        QPen readpen(Qt::red, 1);



        // Create Graph Area
        painter.drawRect(graphbr);
        painter.fillRect(graphbr, QColor(255,255,255));


        // Create Axis / Labeling
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 8));
        painter.scale(1.0,1.0);

        // Y Axis Labeling
        QPainter paintertext(this);
        paintertext.setPen(Qt::black);
        paintertext.setFont(QFont("Arial", 8));
        paintertext.scale(1.0,1.0);
        paintertext.drawText(0,i_y_d_size+10, xstring1);
        paintertext.drawText(0,10+(i_y_d_size*0.75), xstring2);
        paintertext.drawText(0,10+(i_y_d_size*0.50), xstring3);
        paintertext.drawText(0,10+(i_y_d_size*0.25), xstring4);
        paintertext.drawText(0,10, xstring5);

        //Draw horizontal lines on the graph
        painter.drawLine(50, 0,
                         i_x_d_size+50, 0);
        painter.drawLine(50, (i_y_d_size*0.25),
                         i_x_d_size+50, (i_y_d_size*0.25));
        painter.drawLine(50, (i_y_d_size*0.50),
                         i_x_d_size+50, (i_y_d_size*0.50));
        painter.drawLine(50, (i_y_d_size*0.75),
                         i_x_d_size+50, (i_y_d_size*0.75));
        painter.drawLine(50, (i_y_d_size),
                         i_x_d_size+50, i_y_d_size);
        painter.drawLine(50, 0,
                         50,   i_y_d_size); // draws left border of the rect black


        painter.drawText(65+50,i_y_d_size+20, "Write traffic");
        painter.drawText(150+50,i_y_d_size+20, "Read traffic");
        painter.drawText(i_x_d_size-40,20+(i_y_d_size), t_string);
        painter.drawText(i_x_d_size/2,20+(i_y_d_size), t_i_string);
        painter.drawText(i_x_d_size-180,15, title);
        QString thrstr;
        char *thrval1 = mhr(thrputr);
        char *thrval2 = mhr(thrputw);
        char *totalval = mhr(thrputw + thrputr);
        thrstr.append("Throughput Read:");
        thrstr.append(thrval1);
        thrstr.append("/min Write:");
        thrstr.append(thrval2);
        thrstr.append("/min Total:");
        thrstr.append(totalval);
        thrstr.append("/min");
        painter.drawText(5+50,15, thrstr);
        QString what;

        what.append( "Host: "+(ldata->hostString));
        //if (ldata->userString != "") {
        if (ldata->userString != "") {
                what.append(", monitoring User "+ ldata->userString+".");
        } else if (ldata->shareString != "") {
                what.append(", monitoring Share "+ ldata->shareString+ ".");
        } else if (ldata->domainString != "") {
                what.append(", monitoring Domain "+ ldata->domainString+ ".");
        } else if (ldata->fileString != "") {
                what.append(", monitoring File "+ ldata->fileString+ ".");
        } else what = "Dryrun mode, simulating traffic.";

        painter.drawText(5+50,35, what);
        free(thrval1);
        free(thrval2);
        free(totalval);
        painter.scale(1.0,1.0);
        painter.setPen(writepen);
        painter.drawRect(writerect);painter.fillRect(writerect, Qt::blue);
        painter.setPen(readpen);
        painter.drawRect(readrect);painter.fillRect(readrect, Qt::red);







        // Paint graphs
        //graphpainter.scale(1.0,1.0);
        QPainter graph_w_painter(this);
        graph_w_painter.setRenderHint(QPainter::Antialiasing);
        QPen graph_w_pen(Qt::blue, 0.01);//blue
        graph_w_painter.translate(i_x_os-(f_zoomfactor*
                                          (i_x_d_size - i_dp_number)), i_y_os);
        graph_w_painter.scale(f_zoomfactor,1.0);
        graph_w_painter.setPen(graph_w_pen);
        graph_w_painter.setBrush(QBrush(Qt::blue, Qt::SolidPattern));//blue
        graph_w_painter.drawPolygon(writepg);



        QPainter graph_r_painter(this);
        graph_r_painter.setRenderHint(QPainter::Antialiasing);
        QPen graph_r_pen(Qt::red, 0.01);//red
        graph_r_painter.translate(i_x_os-(f_zoomfactor*
                                          (i_x_d_size - i_dp_number)), i_y_os);
        graph_r_painter.scale(f_zoomfactor,1.0);
        graph_r_painter.setPen(graph_r_pen);
        graph_r_painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));//red
        graph_r_painter.drawPolygon(readpg);


        graph_r_painter.scale(1.0,1.0);
        graph_w_painter.scale(1.0,1.0);

        painter.setPen(Qt::black);





}

void Graph::paintGraph(QPaintEvent *){



}





void Graph::wheelEvent(QWheelEvent *event)
{
        // qDebug() << "Wheel event detected";
        int y =  event->delta();
        g_change_dp_num(y);
}



char *Graph::mhr( long long int kb )
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


/*
 * Get the available size for the Graph widget after a resize event
 */
void Graph::g_get_size(int i_width, int i_height){
        i_get_width = i_width;
        i_get_height = i_height;

        //i_x_d_size = i_width ;
        //i_y_d_size = i_height - 92;

        qDebug()<< "gWidth: "<< i_get_width;
        qDebug()<< "gHeight: "<< i_get_height;
        qDebug()<< "Own Width: "<< this->width();
        qDebug()<< "Own Height: "<< this->height();


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






#include "graph.moc"
