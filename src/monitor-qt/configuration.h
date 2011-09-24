 #ifndef CONFIGURATION_H
 #define CONFIGURATION_H
 #include <QApplication>
 #include <QPushButton>
 #include <QWidget>
 #include <QDebug>
 #include <QProcess>
 #include <QGridLayout>
 #include <QTabWidget>
 #include <QLabel>

 class Configuration : public QWidget
 {

	Q_OBJECT


 public:
     ~Configuration();
     Configuration(QWidget *parent=0);

      QWidget  *configwidget;
      QPushButton *remoteconfigbutton;
      QGridLayout *configgridlayout;
      QLabel *message1, *message2;
     

};

#endif

