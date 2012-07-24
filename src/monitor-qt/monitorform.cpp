#include "monitorform.h"
#include "ui_monitorform.h"

#include <QProgressDialog>
#include <QTest>
#include <QMessageBox>
MonitorForm::MonitorForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::MonitorForm)
{
        ui->setupUi(this);


        /*
         * Start instances of timerClassW (handles the timing of the graph
         * updates) and processRunnerW (handles the smbtamonitor-gen) in
         * own threads
        */
        timeClassW = new Timeclass();
        processRunnerW = new Processrunner();
        QThread processThread, timerThread;
        timeClassW->moveToThread(&timerThread);
        processRunnerW->moveToThread(&processThread);

        /*
         * Create classes d_points (holds the data points) and
         * p_graph (creates the graph
        */
        d_points = new DPoint;
        p_graph = new Graph;

        /*
         * l_writestack, l_readstack store input from socket until they are
         * sent to Visual or D_Point
         */

        l_writestack = new unsigned long; *l_writestack = 0;
        l_readstack = new  unsigned long; *l_readstack = 0;


        monitorSocket = new QLocalSocket();
        running=false;
        layouted = false;

        pid_string = new QString;
        readstring = new QString;
        leftoverstring = new QString;
        configString = new QString;
        readlist = new QStringList;


        connect(ui->startButton, SIGNAL(clicked()),this,
                SLOT(startmonitor()), Qt::UniqueConnection);
        connect(ui->stopButton, SIGNAL(clicked()),this,
                SLOT(stopmonitor()), Qt::UniqueConnection);





        /*
         * Setup graph box
        */
        visualW = new Visual(ui->visual_widget, 5);
        QHBoxLayout *vlayout = new QHBoxLayout(ui->visual_widget);
        vlayout->addWidget(visualW);




}

MonitorForm::~MonitorForm()
{
        delete ui;
}


void MonitorForm::startmonitor()
{

        /*
         * Start and control the monitor process and the painting of the graph
         * visualW is a instance of Visual class where the painting happens
         * monitorsocket connects to the local socket where the the
         * smbtamonitor-gen stores the traffic information timeclassW->timer
         * triggers the graph update
         * **wip**
         */

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



                //         visualW->vs_processnumbers(l_writestack, l_readstack);

                //       qDebug()<<"processrunner->monitorprocess->state() 1:"<< processRunnerW->monitorprocess->state();
                //       processRunnerW->monitorprocess->start("./smbtamonitor-gen -i 3491 -h 10.10.0.81 -u holger -I 0");
                //       processRunnerW->monitorprocess->start("smbtamonitor-gen -x");

                ////
                // The following line triggers all the Child Process errors...
                // When I leave out exec() from Processrunner::run(), the errors disappear
                // but I think it does not run in a different thread then , so I leave it there for now
                // The errors are ugly but they on't stop the program

                /** indicate some processing for the user */

                QProgressDialog *Progress = new QProgressDialog("Connecting to smbtad...", NULL, 0, 0, this);
                Progress->move(this->width()/2, this->height()/2);
                Progress->show();




                processRunnerW->monitorprocess->start("smbtamonitor-gen "+(*configString));


                *pid_string = QString::number(processRunnerW->monitorprocess->pid());
                QString socketString = QString("/var/tmp/smbtamonitor-gen-socket-").append(*pid_string);
                this->update();
                /**
   * wait for 5 seconds, then connect to the socket
   */

                QTime dieTime = QTime::currentTime().addSecs(1);
                while( QTime::currentTime() < dieTime ) {
                        Progress->update();
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                }


                if (!QFile::exists(socketString)) {
                        QMessageBox::information(this,"Error!","Unable to connect to the socket of the subprocess.");
                        delete Progress;
                        exit(1);
                }
                Progress->hide();
                delete Progress;


                ////
                // Test if the socket is already established - the sleep command a few lines earlier should make it work reliable.
                if(QFile::exists(socketString))
                {
                        qDebug() << "Socket found";
                }
                else(qDebug()<<"No socket found");



                monitorSocket->connectToServer(socketString,
                                               QIODevice::ReadOnly);
                connect(timeClassW->timer, SIGNAL(timeout()),
                        this, SLOT(sendtovisualizer()));
//                connect(timeClassW->timer, SIGNAL(timeout()),
//                        this, SLOT(sendtopoints()));
                connect(monitorSocket, SIGNAL(readyRead()),
                        this, SLOT(readfromsocket()));
                qDebug() << monitorSocket->state();


 //               qDebug()<<"processrunner->monitorprocess->state() 2:"<< processRunnerW->monitorprocess->state();
                //       connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()), Qt::UniqueConnection);

        }




}

void MonitorForm::stopmonitor()
{

        ////
        // Stop the graph and the monitor process
        if(running==true){
                qDebug() << "stopmonitor";
                running = false;
                //        processRunnerW->monitorprocess->kill();
                processRunnerW->monitorprocess->terminate();
                //                qDebug()<<"processrunner->monitorprocess->state() 3:"<< processRunnerW->monitorprocess->state();
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



        //    qDebug() << "readfromsocket()";

        *readstring = monitorSocket->readAll();
//            qDebug()<<"*readstring: " << *readstring;
        *readlist = readstring->split("#");
        //    qDebug()<< "Last index: " << readlist->count();

        //    int kk = readlist->count();

        for(int i=0; i < readlist->count()-1; i++){

                QString output = readlist->at(i);

                //        qDebug()<<"Output " << i <<": " << output;

                if(output.startsWith("W")){
                        output.replace("W:","");
                        output.chop(1);
                        *l_writestack += output.toULong();
                }

                if(output.startsWith("R")){
                        output.replace("R:","");
                        output.chop(1);

                        *l_readstack += output.toULong();
                }




                //                qDebug()<<i;
                //                qDebug()<<readlist->at(i);
                //                qDebug()<<"--------------";

        }

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
  //         qDebug()<<"*l_readstack ->visualizer " << *l_readstack;
        //    qDebug()<<"*l_writestack " << *l_writestack;
        visualW->vs_receivenumbers( l_readstack, l_writestack);
        d_points->dp_receivenumbers( l_readstack, l_writestack);
        p_graph->g_receivelist(d_points->dp_returnreadlist(),
                               d_points->dp_returnwritelist());

        *l_readstack = 0;
        *l_writestack = 0;

        mutex.unlock();



}


// todo
void MonitorForm::sendtopoints(){


        ////
        // Send input from socket, stored in i_writestack and i_readstack to the graph-painting widget visualW (visual.cpp)
        // Work with dummy input for now as the traffic from the local domain socket can not be read at the moment
        // **not finished yet**
        // **works with dummy numbers atm**

        mutex.lock();


        //    qDebug() << "sendtovisualizer()";
        //    unsigned long int uli1 = 1000;
        //    unsigned long int uli2 = 1500;
//            qDebug()<<"*l_readstack -> topoints " << *l_readstack;
        //    qDebug()<<"*l_writestack " << *l_writestack;
        d_points->dp_receivenumbers( l_readstack, l_writestack);
        p_graph->g_receivelist(d_points->dp_returnreadlist(), d_points->dp_returnwritelist());
        *l_readstack = 0;
        *l_writestack = 0;

        mutex.unlock();



}





void MonitorForm::resizeEvent(QResizeEvent *event)
{
        //  qDebug()<<"Monitorform Resized";

}




#include "monitorform.moc"
