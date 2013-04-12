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
#include <QTime>

#include "instancedata.h"
#include <talloc.h>
#include "../../include/version.h"


class Graph : public QWidget
{
        Q_OBJECT
public:
        explicit Graph(InstanceData *idata, QWidget *parent = 0);

        InstanceData *ldata;

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

        int i_get_width; // Get available width after a resizing event
        int i_get_height; // Get available height after a resizing event

        float f_scalefactor;
        float f_zoomfactor; // Keeps the zoomfactor (i_x_d_size/i_dp_num)

        long l_read_max, l_write_max, l_max, l_c_max; // Holds the max values to do the scaling for the graph
        long l_read_diff, l_write_diff; // Needed to calculate the interpolation steps
        long l_read_prec, l_write_prec; // Needed to calculate the interpolation steps
        unsigned long thrputr;
        unsigned long thrputw;

        //    QLinkedList<unsigned long> readlist, writelist; // Holds traffic values from dpoints
        QList<unsigned long> *readlist, *writelist; // Holds traffic values from dpoints
        QList<long long> readlist_w, writelist_w, worklist;
        //QList<unsigned long> readlist_int, writelist_int;
        QPointF readp, writep;
        QPolygonF readpg, writepg; //Stores the QPointFs for the pQPinterpaths
        QString xstring1, xstring2, xstring3, xstring4, xstring5; // Graph axis labels
        QString t_string, t_i_string; // Holds current time and time for full diagramm width
        QString title; // Holds the title displayed in the graph
        QTime g_clock;

        // Hold configuration data
        QString  *hostString, *portString, *shareString, *userString,
        *domainString, *fileString;



signals:

public slots:

        void g_receivelist(QList<unsigned long> readlist_in,
                           QList<unsigned long> writelist_in);
        QList<long long> g_prepare_data(QList<unsigned long> getlist);
        void g_get_w_size(); // Get the size of the widget that contains the graph
        int  g_get_dp_offset(); // Get the datapoint offset to t=0
        void g_change_dp_num(int i_delta); // Change the number of datapoints for the graph
        void g_def_dp_num(); // Map datapoints to pixels
        void g_release_dp(); // map data points to pixel width (dp < pw)
        void g_squeeze_dp(); // map data points to pixel width (dp > pw)
        void g_interpolate(QList<unsigned long> readlist_in,
                           QList<unsigned long> writelist_in); // Inteerpolation and create points to make the graph
        void g_create_path(QList<unsigned long> readlist_int,
                           QList<unsigned long> writelist_int);
        void g_get_size(int i_width, int i_height);
        char *mhr( long long int kb );

        void paintEvent(QPaintEvent *event);
        void paintGraph(QPaintEvent *event);
        void wheelEvent(QWheelEvent *event); // Will catch wheel events to zoom the graph





};

#endif // GRAPH_H
