 #ifndef SMBTAMONITOR_RUN_H
 #define SMBTAMONITOR_RUN_H

 #include "include.h"
 #include "timeclass.h"
 #include "visual.h"
 #include "processrunner.h"
 
 
 
 class Smbtamonitor_run : public QThread
 {

	Q_OBJECT


 public:
     ~Smbtamonitor_run();
     Smbtamonitor_run(QWidget *parent=0);
//     QProcess *monitorprocess;
     Timeclass *timeclass;
     Visual *visualizer;
     Processrunner *processrunner;
     
     int i_debug;
     
     QString *output;
     QLabel *outputline;
     
     
     void run();
     
 
 public slots:
     
     void smr_timersignal();
     void smr_visualizer();
     void smr_sendmessage();
     void smr_parsemonitor();


};

#endif

 
