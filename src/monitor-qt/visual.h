 
 #ifndef VISUAL_H
 #define VISUAL_H

 #include "include.h"
#include <talloc.h>

 class Visual : public QWidget
 {

	Q_OBJECT


 public:
     ~Visual();
     Visual(QWidget *parent);
     
//     QPainter *painter;
     
     QWidget *visualwidget;
     QVBoxLayout *visuallayout;
     QLabel *visuallabel, *visualwritevalue, *visualreadvalue, *visualhistorymax, *visualtimerstep;
     unsigned long *l_visualread, *l_visualwrite, *l_currentmax, *l_timerstep;
     unsigned long  *l_historymax;
     QString xstring1, xstring2, xstring3, xstring4, xstring5;
     int i_time, i_scalefactor, i_oldscalefactor;
     QVector<QPointF> readv, writev;
     QPointF readp, writep;
     QLine readl, writel;
     QPolygon readpg, writepg;
     QPainterPath readpath, writepath;
	char *mhr( TALLOC_CTX *ctx, long long int kb );  
     void vs_processnumbers(unsigned long *l_read, unsigned long *l_write);
     void paintEvent(QPaintEvent *event);


};

#endif

 
