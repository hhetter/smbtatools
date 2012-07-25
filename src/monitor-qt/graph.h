#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QLinkedList>
#include <QPointF>
#include <QPolygonF>
#include <QList>
#include <QDebug>
#include <QSize>
#include <QPoint>
#include <QString>
#include <QWheelEvent>
#include <QPainter>
#include <talloc.h>


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
        int i_x_os; // Stores the diagram x offset (Distance widget border - diagram)
        int i_y_os; // Stores the diagram y offset (Distance widget border - diagram)
        int i_x_w_size; // Stores width of the widget that holds the diagram - fixed for now, will change dynamically in the future
        int i_y_w_size; // Stores height of the widget that holdsdiagram - fixed for now, will change dynamically in the future
        int i_dp_number; // Number of data points to be displayed in the graph - initialla i_dp_number = i_x_d_size
        int i_dp_start, i_dp_end; // Defines index of the datapoints that will appear on the graph
        int i_dp_min; // Minimum number of datapoints to be displayed in the graph
        int i_dp_max; // Maximum number of datapoints to be displayed in the graph
        int i_intpol_count; // Counter for the interpolation steps
        float f_scalefactor;

        unsigned long l_read_max, l_write_max, l_max, l_c_max; // Holds the max values to do the scaling for the graph
        unsigned long l_read_diff, l_write_diff; // Needed to calculate the interpolation steps


        //    QLinkedList<unsigned long> readlist, writelist; // Holds traffic values from dpoints
        QList<unsigned long> *readlist, *writelist; // Holds traffic values from dpoints
        QPointF readp, writep;
        QPolygonF readpg, writepg; //Stores the QPointFs for the pQPinterpaths
        QString xstring1, xstring2, xstring3, xstring4, xstring5; // Graph axis labels


signals:

public slots:

        void g_receivelist(QList<unsigned long> readlist_in, QList<unsigned long> writelist_in);
        void g_get_w_size(); // Get the size of the widget that contains the graph
        int  g_get_dp_offset(); // Get the datapoint offset to t=0
        void g_change_dp_num(int i_delta); // Change the number of datapoints for the graph
        void g_def_dp_num(); // Map datapoints to pixels
        void g_release_dp(); // map data points to pixel width (dp < pw)
        void g_squeeze_dp(); // map data points to pixel width (dp > pw)
        void g_interpolate(QList<unsigned long> readlist_in, QList<unsigned long> writelist_in); // Inteerpolation and create points to make the graph
        void g_create_path();
        char *mhr( long long int kb );

        void paintEvent(QPaintEvent *event);
        void paintGraph(QPaintEvent *event);
        void wheelEvent(QWheelEvent *event); // Will catch wheel events to zoom the graph




};

#endif // GRAPH_H