

 #include "frontend.h"
 #include "configuration.h"
 #include "smbtamonitor-run.h"
 #include "visual.h"
 // #include "Scriptfunc.h"
 #include <QApplication>
 #include <QFont>
// #include <QLCDNumber>
 #include <QPushButton>
// #include <QSlider>
 #include <QComboBox>
// #include <QVBoxLayout>
 #include <QWidget>
 #include <QTextEdit>
 #include <QFile>
 #include <QIODevice>
 #include <QProcess>
 #include <QGridLayout>

// Main function
 int main(int argc, char *argv[])
 {
     QApplication app(argc, argv);
     Frontend frontendwidget;
//     qDebug("bleh");
     
 //    frontendwidget.i_frontendvariable = 5;
 //    Testclass testclass;
  //   testclass.getamessage();
 //    widget.i = 1+2;
     frontendwidget.resize(650,650);
     frontendwidget.show();
     return app.exec();
 }
