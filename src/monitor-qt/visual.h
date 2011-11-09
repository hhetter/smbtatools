 
 #ifndef VISUAL_H
 #define VISUAL_H

 #include "include.h"

 class Visual : public QWidget
 {

	Q_OBJECT


 public:
     ~Visual();
     Visual(QWidget *parent=0);
     
     QWidget *visualwidget;
     QGridLayout *visuallayout;
     QLabel *visuallabel;


};

#endif

 
