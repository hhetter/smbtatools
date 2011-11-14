 
 #ifndef VISUAL_H
 #define VISUAL_H

 #include "include.h"

 class Visual : public QWidget
 {

	Q_OBJECT


 public:
     ~Visual();
     Visual(QWidget *parent);
     
     QWidget *visualwidget;
     QGridLayout *visuallayout;
     QLabel *visuallabel, *visualwritevalue, *visualreadvalue;;
     int *i_visualread, *i_visualwrite;


};

#endif

 
