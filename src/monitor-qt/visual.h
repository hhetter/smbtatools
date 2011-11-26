 
 #ifndef VISUAL_H
 #define VISUAL_H

 #include "include.h"
#include <talloc.h>

 class Visual : public QWidget
 {

	Q_OBJECT


 public:
     ~Visual();
     Visual( QWidget *parent);
     
//     QPainter *painter;
     
     QWidget *visualwidget;
     QVBoxLayout *visuallayout;
     QLabel *visuallabel, *visualwritevalue, *visualreadvalue, *visualhistorymax, *visualtimerstep;
     unsigned long *l_visualread, *l_visualwrite, *l_currentmax, *l_timerstep;
     unsigned long  *l_historymax;
     QString xstring1, xstring2, xstring3, xstring4, xstring5;
     int i_time, i_scalefactor, i_oldscalefactor;
     float f_scalefactor, f_oldscalefactor;
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

#endif

 
