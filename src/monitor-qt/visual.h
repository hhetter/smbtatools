 
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
     
     
     void vs_processnumbers(long *l_read, long *l_write);
     void vs_painter();


};

#endif

 
