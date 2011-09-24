#ifndef FRONTEND_H
#define FRONTEND_H
 #include "configuration.h"
 #include <QApplication>
 #include <QPushButton>
 #include <QWidget>
 #include <QDebug>
 #include <QProcess>
 #include <QGridLayout>

 class Frontend : public QWidget
 {

	Q_OBJECT


 public:
     Frontend(QWidget *parent=0);
     ~Frontend();

     int i_frontendvariable;

     QPushButton *configbutton, *monitorbutton, *quitbutton;
     QGridLayout *gridlayout;
     
     Configuration *configurator;

 public slots:
//     void gt_getdevices();

};

#endif
