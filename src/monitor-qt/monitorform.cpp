#include "monitorform.h"
#include "ui_monitorform.h"

MonitorForm::MonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonitorForm)
{
    ui->setupUi(this);


    timeClassW = new Timeclass();
    timeClassW->start();
    processRunnerW = new Processrunner();
    processRunnerW->start();
    running=false;



    //    timeClassW->start();
    connect(ui->startButton, SIGNAL(clicked()),this, SLOT(startmonitor()), Qt::UniqueConnection);
    connect(ui->stopButton, SIGNAL(clicked()),this, SLOT(stopmonitor()), Qt::UniqueConnection);
//    connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()));



    i_debug=0;
    l_writestack = new unsigned long; *l_writestack = 0;
    l_readstack = new  unsigned long; *l_readstack = 0;



}

MonitorForm::~MonitorForm()
{
    delete ui;
}


void MonitorForm::startmonitor()
{


    if(running == false){
       qDebug() << "startmonitor";
       running=true;
       timeClassW->timer->start();

       timeClassW->tc_timersignal();
//       visualW = new Visual(ui->visual_widget, 5);

       QLabel testlabel("BLah", ui->visual_widget);
       testlabel.setText("Blah");
       testlabel.setParent(ui->visual_widget);
//       visualW->show();

//       ui->label->setText("Derp");
//         visualW->vs_processnumbers(l_writestack, l_readstack);
       qDebug()<<"processrunner->monitorprocess->state() 1:"<< processRunnerW->monitorprocess->state();
//       processRunnerW->monitorprocess->start("./owntools3");
       qDebug()<<"processrunner->monitorprocess->state() 2:"<< processRunnerW->monitorprocess->state();
       connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()));

    }

//    timeClassW->start();
//    processRunnerW->start();
//    processRunnerW->monitorprocess->start("./owntools3");

//    processRunnerW->monitorprocess->start("./owntools3");
//    connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()));




}

void MonitorForm::stopmonitor()
{



//    timeClassW->start();
    if(running==true){
//    if(1){
        qDebug() << "stopmonitor";
        running = false;
        processRunnerW->monitorprocess->kill();
        qDebug()<<"processrunner->monitorprocess->state() 3:"<< processRunnerW->monitorprocess->state();
        timeClassW->timer->stop();
    }
//    sleep(1);

//    processRunnerW->monitorprocess->start("./owntools3");
//    sleep(100);
//    connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), qApp, SLOT(quit()));




}



void MonitorForm::sendmessage()
{

    qDebug() << "monitorform:sendmessage()";

}

#include "monitorform.moc"
