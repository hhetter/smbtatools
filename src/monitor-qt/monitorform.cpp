#include "monitorform.h"
#include "ui_monitorform.h"

MonitorForm::MonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonitorForm)
{
    ui->setupUi(this);


    timeClassW = new Timeclass();
    processRunnerW = new Processrunner();



    //    timeClassW->start();
    connect(ui->startButton, SIGNAL(clicked()),this, SLOT(startmonitor()));
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


    timeClassW->start();
    processRunnerW->start();
//    processRunnerW->monitorprocess->start("./owntools3");
//    connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), qApp, SLOT(quit()));




}


void MonitorForm::sendmessage()
{

    qDebug() << "sendmessage";

}

#include "monitorform.moc"
