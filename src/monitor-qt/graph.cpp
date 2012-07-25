#include "graph.h"

Graph::Graph(QWidget *parent) :
    QWidget(parent)
{
    i_max_index = 86400;
    i_s_count = 0;
    i_stepsize=5;
    i_x_d_size = 400;
    i_dp_number = i_x_d_size;


    i_dp_start = 0; // index of the data point from where the display is started
    i_dp_end = i_dp_number + i_dp_start; // index of the data point from where the display  ends

    readlist = new QList<unsigned long>;
    writelist = new QList<unsigned long>;
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



void Graph::g_change_dp_num() // Change the number of datapoints for the graph
{
    if(/* signal to zoom in*/ true)
    {
        i_dp_number = (int)(i_dp_number*0.8);
    }

    if(/* signal to zoom out*/ true)
    {
        i_dp_number = (int)(i_dp_number*1.2);
    }
    i_dp_start = i_dp_start + g_get_dp_offset();
    i_dp_end = i_dp_start + i_dp_end;
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
    // Find combined max value

        l_max = 0;
    for(int i = i_dp_start; i < i_dp_end; i++){
            l_c_max = readlist_in[i] + writelist_in[i];
        if(l_c_max > l_max){
            l_max = l_c_max;
        }
    }
    qDebug() << "lmax: " << l_max;
//      qDebug() << "readlist[0]: " << *readlist[0];

}


void Graph::g_create_path()
{




}




void Graph::wheelEvent(QWheelEvent *event)
{
    qDebug() << "Wheel event detected";
}



#include "graph.moc"
