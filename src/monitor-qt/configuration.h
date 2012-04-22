#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>

class Configuration : public QWidget
{

       Q_OBJECT


public:
    ~Configuration();
//    Configuration(QWidget *parent=0);
    Configuration();

     QWidget  *configwidget;
     QPushButton *remoteconfigbutton;
     QGridLayout *configgridlayout;
     QLabel *message1, *message2;
     QFormLayout *configform;
     QLineEdit *hostline, *portline, *shareline, *timeline, *timeframe;
     QComboBox *sharebox;


};

#endif





/*

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDebug>


class Configuration
{
public:
    Configuration();
    ~Configuration();
};

#endif // CONFIGURATION_H
*/
