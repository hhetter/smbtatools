 #ifndef CONFIGURATION_H
 #define CONFIGURATION_H

 #include "include.h"

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
      QFormLayout *configform;
      QLineEdit *hostline, *portline, *shareline, *timeline, *timeframe;
      QComboBox *sharebox;
     

};

#endif

