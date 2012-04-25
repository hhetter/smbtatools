#include "monitorform.h"
#include "ui_monitorform.h"

MonitorForm::MonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonitorForm)
{
    ui->setupUi(this);


    timeClassW = new Timeclass();
    processRunnerW = new Processrunner();
    running=false;



    //    timeClassW->start();
    connect(ui->startButton, SIGNAL(clicked()),this, SLOT(startmonitor()));
    connect(ui->stopButton, SIGNAL(clicked()),this, SLOT(stopmonitor()));
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

    qDebug() << "startmonitor";
    running=true;

    timeClassW->start();
    processRunnerW->start();
    processRunnerW->monitorprocess->start("./owntools3");
//    sleep(1);

//    processRunnerW->monitorprocess->start("./owntools3");
//    sleep(100);
    connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()));




}

void MonitorForm::stopmonitor()
{

    qDebug() << "stopmonitor";


//    timeClassW->start();
    if(running==true){
    processRunnerW->monitorprocess->close();
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
