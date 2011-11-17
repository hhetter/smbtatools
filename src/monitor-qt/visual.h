 
 #ifndef VISUAL_H
 #define VISUAL_H

 #include "include.h"

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
     int i_time;
     QVector<QPoint> readv, writev;
     QPoint readp, writep;
     QLine readl, writel;
     
     void vs_processnumbers(unsigned long *l_read, unsigned long *l_write);
     void paintEvent(QPaintEvent *event);


};

#endif

 
