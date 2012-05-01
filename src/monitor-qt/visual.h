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
    unsigned long  *l_historymax;
    unsigned long l_readmax, l_writemax;
    QString xstring1, xstring2, xstring3, xstring4, xstring5;
    int i_time, i_scalefactor, i_oldscalefactor, i_rescaletimer, f_switch;
    float f_scalefactor, f_oldscalefactor;
    int i_stepping; // To create the size of a step between following data sets in the graph
    int i_x_os, i_y_os;   // Offset for x- and y-Graph
    int i_x_max, i_y_max; // Range of x- and y-Graph
    int i_step; // used to make variable step size for variable graphs
    int i_x;
    QVector<QPoint> readv, writev; //QVector<QPointF> readv, writev;
    QPoint readp, writep; // QPointF readp, writep;
    QLine readl, writel;
    QPolygonF readpg, writepg; //QPolygon readpg, writepg;
    QPainterPath readpath, writepath;



    char *mhr( long long int kb );
    void vs_processnumbers(unsigned long *l_read, unsigned long *l_write);
    void paintEvent(QPaintEvent *event);
    void vs_wraptraffic(unsigned long *l_read, unsigned long *l_write, int i_timeframe);


};

#endif // VISUAL_H
