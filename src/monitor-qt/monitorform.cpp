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

    testsocketprocess = new QProcess;

//    int i_listmax;


    //    timeClassW->start();
    connect(ui->startButton, SIGNAL(clicked()),this, SLOT(startmonitor()), Qt::UniqueConnection);
    connect(ui->stopButton, SIGNAL(clicked()),this, SLOT(stopmonitor()), Qt::UniqueConnection);




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

    ////
    // Start and control the monitor process and the painting of the graph
    // visualW is a instance of Visual class where the painting happens
    // monitorsocket connects to the local socket where the the smbtamonitor-gen stores the traffic information
    // timeclassW->timer triggers the graph update
    // **problem: signal QIODevice::readyRead() seems to never get emitted**



    if(running == false){
       qDebug() << "startmonitor";
       running=true;
       qDebug()<<"1";
       timeClassW->timer->start();


//       timeClassW->tc_timersignal();

       visualW = new Visual(ui->visual_widget, 5);
       QHBoxLayout *vlayout = new QHBoxLayout(ui->visual_widget);

       vlayout->addWidget(visualW);



//         visualW->vs_processnumbers(l_writestack, l_readstack);
       qDebug()<<"processrunner->monitorprocess->state() 1:"<< processRunnerW->monitorprocess->state();
       processRunnerW->monitorprocess->start("./smbtamonitor-gen -i 3491 -h 10.10.0.81 -u holger -I 0");
//       testsocketprocess->start("./smbtamonitor-gen -i 3491 -h 10.10.0.81 -u holger -I 0");



       *pid_string = QString::number(processRunnerW->monitorprocess->pid());
       QString socketString = QString("/var/tmp/smbtamonitor-gen-socket-").append(*pid_string);
       sleep(1);
       qDebug()<<socketString;
       if(QFile::exists(socketString))
       {
           qDebug() << "Socket found";

       }
       else(qDebug()<<"No socket found");

//       connect(monitorSocket, SIGNAL(connected()), this, SLOT(sendmessage()));
//       connect(monitorSocket, SIGNAL(readyRead()), this, SLOT(sendmessage()));
//       connect(monitorSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendmessage()));

//       connect(monitorSocket, SIGNAL(readyRead()), this, SLOT(readfromsocket()));
//       connect(timeClassW->timer, SIGNAL(timeout()), this, SLOT(sendtovisualizer()));

       //connect(timeClassW->timer, SIGNAL(timeout()), this, SLOT(sendmessage())); // enter function here: QIODevice::readData


       monitorSocket->connectToServer(socketString, QIODevice::ReadOnly);
       connect(timeClassW->timer, SIGNAL(timeout()), this, SLOT(sendtovisualizer()));
       connect(monitorSocket, SIGNAL(readyRead()), this, SLOT(sendmessage()));
       qDebug() << monitorSocket->state();


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

    ////
    // Stop the graph and the monitor process




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
    qDebug()<< "Heute: readyRead()";

}

void MonitorForm::readfromsocket(){

    ////
    // Read input from the socket and write to i_readstack and i_writestack
    // Test if traffic line is complete (trailing '#') and handle incomplete traffic values
    // **not finished yet**



    qDebug() << "readfromsocket()";

    *readstring = monitorSocket->readAll();
    qDebug()<<"*readstring: " << *readstring;
    *readlist = readstring->split("#");
    qDebug()<< "Last index: " << readlist->count();

//    int kk = readlist->count();

//    for(int i=0; i < readlist->count(); i++){
//                qDebug()<<i;
//                qDebug()<<readlist->at(i);
//                qDebug()<<"--------------";

//            }
//
//    *readstring = "";

}



void MonitorForm::parseinput(){

    ////
    // Might handle parsing of socket input here, now it is located in readfromsocket()
    // **not started yet**



}

void MonitorForm::sendtovisualizer(){


    ////
    // Send input from socket, stored in i_writestack and i_readstack to the graph-painting widget visualW (visual.cpp)
    // Work with dummy input for now as the traffic from the local domain socket can not be read at the moment
    // **not finished yet**
    // **works with dummy numbers atm**

    mutex.lock();


    qDebug() << "sendtovisualizer()";
    unsigned long int uli1 = 1000;
    unsigned long int uli2 = 1500;
    visualW->vs_processnumbers( &uli1, &uli2);



    mutex.unlock();



}




#include "monitorform.moc"
