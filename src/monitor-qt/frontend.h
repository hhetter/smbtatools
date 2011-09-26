#ifndef FRONTEND_H
#define FRONTEND_H

 #include "include.h"
 #include "configuration.h"
 #include "smbtamonitor-run.h"
 #include "visual.h"

//   Class Configuration;
//   Class Visual;

 class Frontend : public QWidget
 {

	Q_OBJECT


 public:
     Frontend(QWidget *parent=0);
     ~Frontend();

     int i_frontendvariable;

     QPushButton *configbutton, *monitorbutton, *quitbutton;
     QGridLayout *gridlayout, *frontendlayout;
     QWidget *frontendwidget;
     
     Configuration *configurator;
     Smbtamonitor_run *smbtamonitur_runner;
     Visual *visualizer;

 public slots:
     void fr_config();
     void fr_getmonitor();
     

};

#endif
