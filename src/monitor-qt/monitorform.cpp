#include "monitorform.h"
#include "ui_monitorform.h"


MonitorForm::MonitorForm(InstanceData *idata, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::MonitorForm)
{
        ui->setupUi(this);
        ldata = idata;


        /*
         * Start instances of timerClassW (handles the timing of the graph
         * updates) and processRunnerW (handles the smbtamonitor-gen) in
         * own threads
        */
        //idata = new InstanceData();
        timeClassW = new Timeclass(idata);
        processRunnerW = new Processrunner(idata);
        QThread processThread, timerThread;
        timeClassW->moveToThread(&timerThread);
        processRunnerW->moveToThread(&processThread);

        /*
         * Create classes d_points (holds the data points) and
         * p_graph (creates the graph
        */
        d_points = new DPoint(idata);
        p_graph = new Graph(idata, ui->visual_widget);

        /*
         * l_writestack, l_readstack store input from socket until they are
         * sent to Visual or D_Point
         */

        l_writestack = new unsigned long; *l_writestack = 0;
        l_readstack = new  unsigned long; *l_readstack = 0;


        monitorSocket = new QLocalSocket();
        running=false;
        layouted = false;
        isConnected = true;

        pid_string = new QString;
        readstring = new QString;
        leftoverstring = new QString;

        readlist = new QStringList;

        // Fetch ALL the configuration data
        configString = new QString;
        hostString   = new QString;
        fileString   = new QString;
        portString   = new QString;
        shareString  = new QString;
        userString   = new QString;
        domainString = new QString;




        connect(ui->startButton, SIGNAL(clicked()),this,
                SLOT(startmonitor()), Qt::UniqueConnection);
        connect(ui->stopButton, SIGNAL(clicked()),this,
                SLOT(stopmonitor()), Qt::UniqueConnection);
        connect(timeClassW->timer, SIGNAL(timeout()),
                this, SLOT(sendtopoints()));
        connect(monitorSocket, SIGNAL(readyRead()),
                this, SLOT(readfromsocket()));





        /*
         * Setup graph box
        */
        //p_graph = new Graph(idata, ui->visual_widget);
        QHBoxLayout *vlayout = new QHBoxLayout(ui->visual_widget);
        vlayout->addWidget(p_graph);


}

MonitorForm::~MonitorForm()
{
        delete ui;
}


void MonitorForm::startmonitor()
{
        p_graph->hostString   = hostString;
        p_graph->fileString   = fileString;
        p_graph->portString   = portString;
        p_graph->shareString  = shareString;
        p_graph->userString   = userString;
        p_graph->domainString = domainString;


        /*
         * Start and control the monitor process and the painting of the graph
         * visualW is a instance of Visual class where the painting happens
         * monitorsocket connects to the local socket where the the
         * smbtamonitor-gen stores the traffic information timeclassW->timer
         * triggers the graph update
         * **wip**
         */

        if(running == false){


                //qDebug() << "startmonitor";


                timeClassW->timer->start();



                //         visualW->vs_processnumbers(l_writestack, l_readstack);


                /** indicate some processing for the user */

                QProgressDialog *Progress = new QProgressDialog("Connecting to smbtad...", NULL, 0, 0, this);
                Progress->move(this->width()/2, this->height()/2);
                Progress->show();




                //processRunnerW->monitorprocess->start("smbtamonitor-gen "+(*configString));

                processRunnerW->monitorprocess->start("smbtamonitor-gen "+(ldata->configString));

                //*pid_string = QString::number(processRunnerW->monitorprocess->pid());
                //QString socketString = QString("/var/tmp/smbtamonitor-gen-socket-").append(*pid_string);

                ldata->pidString = QString::number(processRunnerW->monitorprocess->pid());
                ldata->socketString = QString("/var/tmp/smbtamonitor-gen-socket-").append(ldata->pidString);

                this->update();
                /**
                * wait for x seconds, then connect to the socket
                */

                QTime dieTime = QTime::currentTime().addSecs(2);
                while( QTime::currentTime() < dieTime ) {
                        Progress->update();
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                }

                if (!QFile::exists(ldata->socketString)) {
                        QMessageBox::information(this,"Error!","Unable to connect to the socket of the subprocess.");
                        isConnected = false;

                }else{
                        isConnected = true;
                        connectSocket();
                };

                Progress->hide();

                delete Progress;


        }




}

void MonitorForm::connectSocket()
{
        if(isConnected){


                running=true;
                monitorSocket->connectToServer(ldata->socketString,
                                               QIODevice::ReadOnly);
                //connect(timeClassW->timer, SIGNAL(timeout()),
                //        this, SLOT(sendtovisualizer()));
                /*
                connect(timeClassW->timer, SIGNAL(timeout()),
                        this, SLOT(sendtopoints()));
                connect(monitorSocket, SIGNAL(readyRead()),
                        this, SLOT(readfromsocket()));
                //qDebug() << monitorSocket->state();
*/

                //               qDebug()<<"processrunner->monitorprocess->state() 2:"<< processRunnerW->monitorprocess->state();
                //       connect(processRunnerW->monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(sendmessage()), Qt::UniqueConnection);
        }
        else(qDebug()<< "Error while connecting to the smbtad");

}

void MonitorForm::stopmonitor()
{

        ////
        // Stop the graph and the monitor process
        if(running==true){
                //qDebug() << "stopmonitor";
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
        //qDebug() << "monitorform:sendmessage()";
        //qDebug()<< "Heute: readyRead()";

}

void MonitorForm::readfromsocket(){

        ////
        // Read input from the socket and write to i_readstack and i_writestack
        // Test if traffic line is complete (trailing '#') and handle incomplete traffic values
        // **trailing '#' issue not touched yet**
        // **not finished yet**


        mutex.lock();
        //    qDebug() << "readfromsocket()";

        *readstring = monitorSocket->readAll();
        //           qDebug()<<"*readstring: " << *readstring;
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
        mutex.unlock();

}



void MonitorForm::parseinput(){

        ////
        // Might handle parsing of socket input here, now it is located in readfromsocket()
        // **not started yet**



}

void MonitorForm::sendtovisualizer(){

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
        //   qDebug()<<"*l_readstack ->visualizer   " << *l_readstack;
        //   qDebug()<<"*l_writestack -> visualizer " << *l_writestack;
        d_points->dp_receivenumbers( l_readstack, l_writestack);
        p_graph->g_receivelist(d_points->dp_returnreadlist(),
                               d_points->dp_returnwritelist());


        //        QList<unsigned long> readlist; readlist = d_points->dp_returnreadlist();



        *l_readstack = 0;
        *l_writestack = 0;

        mutex.unlock();



}





void MonitorForm::resizeEvent(QResizeEvent *event)
{
          qDebug()<<"Monitorform Resized";

}




#include "monitorform.moc"
