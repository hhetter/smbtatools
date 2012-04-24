 #ifndef TIMECLASS_H
 #define TIMECLASS_H

 #include "include.h"

 class Timeclass : public QThread
 {

	Q_OBJECT


 public:
     ~Timeclass();
     Timeclass(QWidget *parent=0);
//     QProcess *monitorprocess;
     
     QTimer *timer; 
     
     void run();
     
      
 public slots:
     
     void tc_timersignal();


};

#endif

 
