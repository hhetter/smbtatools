#include "graph.h"

Graph::Graph(QWidget *parent) :
    QWidget(parent)
{
        i_s_count = 0;
        i_stepsize=5;
        i_x_d_size = 400;
        i_dp_number = i_x_d_size;
}



void Graph::g_receivelist(QLinkedList<unsigned long> readlist,                           QLinkedList<unsigned long> writelist)
 {
                qDebug()<<"g_receivelist.readlist.size: "<< readlist.size();
                g_get_w_size();


 }


/*
 * Get the size of the widget that contains the graph
 * Basic implementation done
 * Widget size doesn't change yet so no visible effect
 * Method not called yet
 */
void Graph::g_get_w_size()
{
    QSize w_size = this->frameSize();
    qDebug()<<"QSize: y " << w_size.height();
    qDebug()<<"QSize calc: y " << (int)(w_size.height()*0.8);
    qDebug()<<"QSize: x " << w_size.width();
}

void Graph::g_get_dp_offset(){} // Get the datapoint offset to t=0



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
}



void Graph::g_def_dp_num() // Map datapoints to pixels
{
    // 1) find out if there are more pixels or more datapoints
    if(i_dp_number <= i_dp_number){
            g_release_dp();
    }
    if(i_dp_number > i_dp_number){
            g_squeeze_dp();
    }
    // 2) find a method to map datapoints to pixels
    // -> see the two methods called from here

}

/*
 * Distribute data points to the graph when the pixel width is greater than the
 * number of data points
 */
void Graph::g_release_dp(){}


/*
 * Distribute data points to the graph when the number of data points is greater
 * than the pixel width.
 */
void Graph::g_squeeze_dp(){}


void Graph::g_interpolate(){} // Interpolation and create points to make the graph







void Graph::wheelEvent(QWheelEvent *event)
{
    qDebug() << "Wheel event detected";
}



#include "graph.moc"
