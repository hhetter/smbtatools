 #ifndef SMBTAMONITOR_RUN_H
 #define SMBTAMONITOR_RUN_H

 #include "include.h"
 #include "timeclass.h"
 #include "visual.h"
 
 
 
 class Smbtamonitor_run : public QThread
 {

	Q_OBJECT


 public:
     ~Smbtamonitor_run();
     Smbtamonitor_run(QWidget *parent=0);
     QProcess *monitorprocess;
     Timeclass *timeclass;
     Visual *visualizer;
     
     
     void run();
     
 
 public slots:
     
     void smr_timersignal();


};

#endif

 
