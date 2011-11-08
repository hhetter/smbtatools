 #ifndef PROCESSRUNNER_H
 #define PROCESSRUNNER_H

 #include "include.h"

 
 
 class Processrunner : public QThread
 {

	Q_OBJECT


 public:
     ~Processrunner();
     Processrunner(QWidget *parent=0);
     QProcess *monitorprocess;
//     Timeclass *timeclass;
//     Visual *visualizer;
     
     
     void run();
     
 
 public slots:
     
     void pr_readsignal();


};

#endif

 
