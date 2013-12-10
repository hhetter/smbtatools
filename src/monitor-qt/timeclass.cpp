#include "timeclass.h"

Timeclass::Timeclass(InstanceData *idata, QWidget *parent) :
    QObject(parent)
{


    i_stepsize = idata->i_stepsize;
    
    timer = new QTimer;
    timer->setInterval(1000/i_stepsize);   // standard value

     connect(timer, SIGNAL(timeout()), this, SLOT(tc_timersignal()));

}


void Timeclass::tc_timersignal(){


}



#include "timeclass.moc"
