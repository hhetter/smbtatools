#ifndef VISUAL_H
#define VISUAL_H

#include <QApplication>
#include <QFont>
#include <QPushButton>
#include <QComboBox>
#include <QWidget>
#include <QFile>
#include <QIODevice>
#include <QProcess>
#include <QGridLayout>
#include <QDebug>
#include <QTabWidget>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QThread>
#include <QString>
#include <QTimer>
#include <QMutex>
#include <QPainter>
#include <QPaintEvent>
#include <QTime>
#include <QFile>

class Visual : public QWidget
{
    Q_OBJECT
public:
    Visual(QWidget *parent, int i_timeframe);
    ~Visual();


//     QPainter *painter;

    QWidget *visualwidget;
    QVBoxLayout *visuallayout;
    QLabel *visuallabel, *visualwritevalue, *visualreadvalue, *visualhistorymax, *visualtimerstep;
    unsigned long *l_visualread, *l_visualwrite, *l_currentmax, *l_timerstep;
    unsigned long *l_in_read, *l_in_write, *l_in_read_start, *l_in_read_end, *l_in_write_start, *l_in_write_end;
    unsigned long *l_read_diff, *l_write_diff;
    unsigned long  *l_historymax;
    unsigned long l_readmax, l_writemax;
    unsigned long l_findmax; // Used while searching for a new maximum value for rescaling the displayed graph
    QString xstring1, xstring2, xstring3, xstring4, xstring5;
    int i_time, i_scalefactor, i_oldscalefactor, i_rescaletimer, f_switch;
    float f_scalefactor, f_oldscalefactor;
    int i_c_count;  // collectorcount => decides when to hand over information to the graph painter
    int i_stepsize; // i_stepsize defines the number of steps that are used to display a single data point
    int i_stepping; // To create the size of a step between following data sets in the graph
    int i_x_savemax; // Define how many data points are stored. After max is reached, the oldest values will subsequently be deleted
    int i_x_os, i_y_os;   // Offset for x- and y-Graph
    int i_x_max, i_y_max; // Range of x- and y-Graph
    int i_step; // used to make variable step size for variable graphs
    int i_x; // data point counter
    int i_ts_pos; // Position of the time scale
    QVector<QPoint> readv, writev; //QVector<QPointF> readv, writev;
    QPoint readp, writep; // QPointF readp, writep;
    QLine readl, writel;
    QPolygonF readpg, writepg; //QPolygon readpg, writepg;
    QPainterPath readpath, writepath;



    char *mhr( long long int kb );
    void vs_processnumbers(unsigned long l_read, unsigned long l_write);
    void vs_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite);
    void paintEvent(QPaintEvent *event);
    void vs_wraptraffic(unsigned long *l_read, unsigned long *l_write, int i_timeframe);
    void wheelEvent(QWheelEvent *event);


};

#endif // VISUAL_H
