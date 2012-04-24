 #ifndef QUERYSHARES_H
 #define QUERYSHARES_H

 #include "include.h"

 
 
 class Queryshares : public QThread
 {

	Q_OBJECT


 public:
     ~Queryshares();
     Queryshares(QWidget *parent=0);
     QProcess *queryprocess;
//     Timeclass *timeclass;
//     Visual *visualizer;
     
     
     void run();
     
 
 public slots:
     
     void qs_readsignal();

};

#endif

 
