 #ifndef SMBTAMONITOR_RUN_H
 #define SMBTAMONITOR_RUN_H

 #include "include.h"

 class Smbtamonitor_run : public QThread
 {

	Q_OBJECT


 public:
     ~Smbtamonitor_run();
     Smbtamonitor_run(QWidget *parent=0);
     QProcess *monitorprocess;
     
     
     void run();
 
 public slots:
     void sr_emit();
 

};

#endif

 
