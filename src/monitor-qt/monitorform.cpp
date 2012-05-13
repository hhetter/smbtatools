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

    monitorSocket = new QLocalSocket();
    running=false;
    pid_string = new QString;
    readstring = new QString;
    readlist = new QStringList;

    int i_listmax;


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
       qDebug()<<"1";
       timeClassW->timer->start();


//       timeClassW->tc_timersignal();

       visualW = new Visual(ui->visual_widget, 5);
       QHBoxLayout *vlayout = new QHBoxLayout(ui->visual_widget);
//       QLabel *label = new QLabel(ui->visual_widget);
 //      label->setText("*label");
       vlayout->addWidget(visualW);

//       visualW = new Visual(ui->visual_widget, 5);
//       QLabel *label = new QLabel(ui->visual_widget);
//       label->setText("label");
//       ui->visual_widget->show();
//       label->setText("hm");
//       long unsigned int uli1 = 1000;
//       visualW->vs_processnumbers(&uli1,&uli1);

//         visualW->vs_processnumbers(l_writestack, l_readstack);
       qDebug()<<"processrunner->monitorprocess->state() 1:"<< processRunnerW->monitorprocess->state();
       processRunnerW->monitorprocess->start("./smbtamonitor-gen -i 3491 -h 10.10.0.81 -u holger -I 0");



       int lpid = processRunnerW->monitorprocess->pid();
       qDebug() <<lpid << " Hey";
//       *pid_string = QString("%1").arg(processRunnerW->monitorprocess->pid());
       *pid_string = QString::number(processRunnerW->monitorprocess->pid());
       QString socketString = QString("/var/tmp/smbtamonitor-gen-socket-").append(*pid_string);
       sleep(1);
       qDebug()<<socketString;
       if(QFile::exists(socketString))
       {
           qDebug() << "Habs";

       }
       else(qDebug()<<"nichts");

       connect(monitorSocket, SIGNAL(connected()), this, SLOT(sendmessage()));
       connect(monitorSocket, SIGNAL(readyRead()), this, SLOT(sendmessage()));
       connect(monitorSocket, SIGNAL(readyRead()), this, SLOT(readfromsocket()));
       connect(timeClassW->timer, SIGNAL(timeout()), this, SLOT(sendtovisualizer()));
       monitorSocket->connectToServer(socketString, QIODevice::ReadOnly);
       qDebug() << monitorSocket->state();


        if(monitorSocket->isValid()){

            qDebug() << "Valide";
        }




       qDebug()<<"processrunner->monitorprocess->state() 2:"<< processRunnerW->monitorprocess->state();
//       connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()), Qt::UniqueConnection);

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



}



void MonitorForm::sendmessage()
{

    qDebug() << "monitorform:sendmessage()";
    qDebug()<< "Heute: connected to socket";

}

void MonitorForm::readfromsocket(){

    qDebug() << "readfromsocket()";

    *readstring = monitorSocket->readLine();
    qDebug()<<*readstring;
    *readlist = readstring->split("#");
    qDebug()<< "Last index: " << readlist->count();

//    int kk = readlist->count();

    for(int i=0; i < readlist->count(); i++){
                qDebug()<<i;
                qDebug()<<readlist->at(i);
                qDebug()<<"--------------";

            }

}



void MonitorForm::parseinput(){



}

void MonitorForm::sendtovisualizer(){

    qDebug() << "sendtovisualizer()";



}




#include "monitorform.moc"
