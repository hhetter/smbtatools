#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QLinkedList>
#include <QDebug>
#include <QSize>


class Graph : public QWidget
{
    Q_OBJECT
public:
    explicit Graph(QWidget *parent = 0);

        int i_s_count;
        int i_stepsize;
        int i_x_d_size; // Stores width of the diagram - fixed for now, will change dynamically in the future
        int i_y_d_size; // Stores height of the diagram - fixed for now, will change dynamically in the future
        int i_x_w_size; // Stores width of the widget that holds the diagram - fixed for now, will change dynamically in the future
        int i_y_w_size; // Stores height of the widget that holdsdiagram - fixed for now, will change dynamically in the future
        int i_dp_number; // Number of data points to be displayed in the graph - initialla i_dp_number = i_x_d_size
        int i_dp_start, i_dp_end; // Defines index of the datapoints that will appear on the graph


signals:

public slots:

        void g_receivelist(QLinkedList<unsigned long> readlist, QLinkedList<unsigned long> writelist);
        void g_get_w_size(); // Get the size of the widget that contains the graph
        int g_get_dp_offset(); // Get the datapoint offset to t=0
        void g_change_dp_num(); // Change the number of datapoints for the graph
        void g_def_dp_num(); // Map datapoints to pixels
        void g_release_dp(); // map data points to pixel width (dp < pw)
        void g_squeeze_dp(); // map data points to pixel width (dp > pw)
        void g_interpolate(); // Inteerpolation and create points to make the graph

        void wheelEvent(QWheelEvent *event); // Will catch wheel events to zoom the graph




};

#endif // GRAPH_H
