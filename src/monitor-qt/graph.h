#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QLinkedList>
#include <QList>
#include <QDebug>
#include <QSize>
#include <QPoint>


class Graph : public QWidget
{
    Q_OBJECT
public:
    explicit Graph(QWidget *parent = 0);

    int i_max_index; // Hold value of maximum arrays
    int i_s_count;
    int i_stepsize;
    int i_x_d_size; // Stores width of the diagram - fixed for now, will change dynamically in the future
    int i_y_d_size; // Stores height of the diagram - fixed for now, will change dynamically in the future
    int i_x_w_size; // Stores width of the widget that holds the diagram - fixed for now, will change dynamically in the future
    int i_y_w_size; // Stores height of the widget that holdsdiagram - fixed for now, will change dynamically in the future
    int i_dp_number; // Number of data points to be displayed in the graph - initialla i_dp_number = i_x_d_size
    int i_dp_start, i_dp_end; // Defines index of the datapoints that will appear on the graph

    unsigned long l_read_max, l_write_max, l_max, l_c_max; // Holds the max values to do the scaling for the graph


//    QLinkedList<unsigned long> readlist, writelist; // Holds traffic values from dpoints
        QList<unsigned long> readlist, writelist; // Holds traffic values from dpoints
    QPoint readp, writep;


signals:

public slots:

    void g_receivelist(QList<unsigned long> readlist_in, QList<unsigned long> writelist_in);
    void g_get_w_size(); // Get the size of the widget that contains the graph
    int  g_get_dp_offset(); // Get the datapoint offset to t=0
    void g_change_dp_num(); // Change the number of datapoints for the graph
    void g_def_dp_num(); // Map datapoints to pixels
    void g_release_dp(); // map data points to pixel width (dp < pw)
    void g_squeeze_dp(); // map data points to pixel width (dp > pw)
    void g_interpolate(); // Inteerpolation and create points to make the graph
    void g_create_path();

    void wheelEvent(QWheelEvent *event); // Will catch wheel events to zoom the graph




};

#endif // GRAPH_H
