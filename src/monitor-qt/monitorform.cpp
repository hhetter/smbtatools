#include "monitorform.h"
#include "ui_monitorform.h"

MonitorForm::MonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonitorForm)
{
    ui->setupUi(this);


    ////
    // Start instances of timerClassW (handles the timing of the graph updates) and processRunnerW (handles the smbtamonitor-gen, maybe in a own thread)
    timeClassW = new Timeclass();
//    timeClassW->start();
    processRunnerW = new Processrunner();
    QThread processThread, timerThread;
    timeClassW->moveToThread(&timerThread);
    processRunnerW->moveToThread(&processThread);
//    processRunnerW->start();

    monitorSocket = new QLocalSocket();
    running=false;
    layouted = false;

    pid_string = new QString;
    readstring = new QString;
    leftoverstring = new QString;
    configString = new QString;
    readlist = new QStringList;



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
    // **wip**
    // **after stopsignal() graph painting is not resumed**


    if(running == false){


        qDebug()<<"Runningfalse";
        qDebug()<<"-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+";
        qDebug()<< "*configstring: " << *configString;
        qDebug()<<"i_monitortime: " << i_monitortime;
        qDebug()<<"i_intervaltime: " << i_intervaltime;
        qDebug()<<"-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+";



       qDebug() << "startmonitor";
       running=true;

       timeClassW->timer->start();



    ////
    // If this has not happened before: instantiate graph widget and add it to layout to make it visible
    if(layouted == false){
        layouted = true;
        visualW = new Visual(ui->visual_widget, 5);
        QHBoxLayout *vlayout = new QHBoxLayout(ui->visual_widget);
        vlayout->addWidget(visualW);
    }


//         visualW->vs_processnumbers(l_writestack, l_readstack);

//       qDebug()<<"processrunner->monitorprocess->state() 1:"<< processRunnerW->monitorprocess->state();
//       processRunnerW->monitorprocess->start("./smbtamonitor-gen -i 3491 -h 10.10.0.81 -u holger -I 0");
//       processRunnerW->monitorprocess->start("smbtamonitor-gen -x");

       ////
       // The following line triggers all the Child Process errors...
       // When I leave out exec() from Processrunner::run(), the errors disappear
       // but I think it does not run in a different thread then , so I leave it there for now
       // The errors are ugly but they on't stop the program
       processRunnerW->monitorprocess->start("smbtamonitor-gen "+(*configString));


       *pid_string = QString::number(processRunnerW->monitorprocess->pid());
       QString socketString = QString("/var/tmp/smbtamonitor-gen-socket-").append(*pid_string);


       while(!QFile::exists(socketString)){
           sleep(1);
       }

       ////
       // Test if the socket is already established - the sleep command a few lines earlier should make it work reliable.
       if(QFile::exists(socketString))
       {
           qDebug() << "Socket found";
       }
       else(qDebug()<<"No socket found");



       monitorSocket->connectToServer(socketString, QIODevice::ReadOnly);
       connect(timeClassW->timer, SIGNAL(timeout()), this, SLOT(sendtovisualizer()));
       connect(monitorSocket, SIGNAL(readyRead()), this, SLOT(readfromsocket()));
       qDebug() << monitorSocket->state();


       qDebug()<<"processrunner->monitorprocess->state() 2:"<< processRunnerW->monitorprocess->state();
//       connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()), Qt::UniqueConnection);

    }




}

void MonitorForm::stopmonitor()
{

    ////
    // Stop the graph and the monitor process




//    timeClassW->start();
    if(running==true){
        qDebug() << "stopmonitor";
        running = false;
        //        processRunnerW->monitorprocess->kill();
        processRunnerW->monitorprocess->terminate();
        qDebug()<<"processrunner->monitorprocess->state() 3:"<< processRunnerW->monitorprocess->state();
        timeClassW->timer->stop();


    }



}



void MonitorForm::sendmessage()
{
    ////
    // used mainly to create debug messages for certain signals


    qDebug() << "monitorform:sendmessage()";
    qDebug()<< "Heute: readyRead()";

}

void MonitorForm::readfromsocket(){

    ////
    // Read input from the socket and write to i_readstack and i_writestack
    // Test if traffic line is complete (trailing '#') and handle incomplete traffic values
    // **trailing '#' issue not touched yet**
    // **not finished yet**



    qDebug() << "readfromsocket()";

    *readstring = monitorSocket->readAll();
    qDebug()<<"*readstring: " << *readstring;
    *readlist = readstring->split("#");
    qDebug()<< "Last index: " << readlist->count();

    //    int kk = readlist->count();

    for(int i=0; i < readlist->count()-1; i++){

        QString output = readlist->at(i);

        qDebug()<<"Output " << i <<": " << output;

        if(output.startsWith("W")){

            //            qDebug()<<"starts with W";
            //            qDebug()<<"------------";
            //                qDebug()<<"Output: " << output;
            output.replace("W:","");
            output.chop(1);
            //                qDebug()<<"Output: " << output;
            *l_writestack += output.toULong();
            qDebug()<<"readfromsocket:*l_writestack: " << *l_writestack;
        }

        if(output.startsWith("R")){
            //            qDebug()<<"starts with R";
            //                qDebug()<<"------------";
            //                qDebug()<<"Output: " << output;
            output.replace("R:","");
            output.chop(1);
            //                qDebug()<<"Output: " << output;
            *l_readstack += output.toULong();
            qDebug()<<"readfromsocket:*l_readstack: " << *l_readstack;
        }




        //                qDebug()<<i;
        //                qDebug()<<readlist->at(i);
        //                qDebug()<<"--------------";

    }
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


//    qDebug() << "sendtovisualizer()";
//    unsigned long int uli1 = 1000;
//    unsigned long int uli2 = 1500;
    qDebug()<<"*l_readstack  " << *l_readstack;
    qDebug()<<"*l_writestack " << *l_writestack;
    visualW->vs_processnumbers( l_readstack, l_writestack);
    *l_readstack = 0;
    *l_writestack = 0;



    mutex.unlock();



}




#include "monitorform.moc"
