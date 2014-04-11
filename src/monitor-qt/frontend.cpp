#include "frontend.h"
#include "ui_frontend.h"


frontend::frontend(InstanceData *idata, QWidget *parent) :
        QWidget(parent),
        //QGraphicsWidget(parent),
        ui(new Ui::frontend)
{
        ui->setupUi(this);
        //this->setWindowTitle("smbtamonitor-qt");
        ////
        // Create the Widgets for configuration and the monitoring

        monitorFormW = new MonitorForm(idata);
        ldata = idata;

        ////
        // Add widgets to the stackedWidget

        ui->stackedView->addWidget(monitorFormW);
        ui->stackedView->show();
        ui->stackedView->setCurrentIndex(3);



        fr_setMonitorWidget();
        /*connect(ui->quitbutton, SIGNAL(clicked()),qApp,
                SLOT(quit()));*/

        fr_firstInit();








}

frontend::~frontend()
{
        delete ui;
}

void frontend::fr_firstInit(){
        /*
        *  Test if ~/.smbtatools exists - if not create it
        *  ~/.smbtatools will store configuration files and stuff
        */
        QString s_path = QDir::homePath().append("/.smbtatools");

        if(!QDir(s_path).exists())
        {
                QDir().mkdir(s_path);
                qDebug()<<"Initial setup: .smbtatools home directory created.";
        }
        if(QDir(s_path).exists() &&
                      !(QFile(s_path.append("/smbtamonitor-qt.conf")).exists()))
        {
                qDebug()<<"Creating Smbtamonitor-qt config file";
                QFile qf_conf_file(s_path);
                qDebug()<< s_path;
                qf_conf_file.open(QIODevice::WriteOnly | QIODevice::Text);
                QTextStream init(&qf_conf_file);
                init << "Smbtamonitor-qt configuration file\n";
                qf_conf_file.close();
        }
}


void frontend::fr_setMonitorWidget()
{       
        //configWidget->cf_readconfig();

        monitorFormW->configString.append(ldata->configString);
        monitorFormW->i_monitortime = ldata->i_monitortime;
        monitorFormW->i_intervaltime = ldata->i_intervaltime;
        monitorFormW->hostString = ldata->hostString;
        monitorFormW->fileString = ldata->fileString;
        monitorFormW->portString = ldata->portString;
        monitorFormW->shareString = ldata->shareString;
        monitorFormW->userString = ldata->userString;
        monitorFormW->domainString = ldata->domainString;

        ui->stackedView->setCurrentIndex(4);
        //    ui->testlabel->setText("fr_setMonitorWidget()");
}

void frontend::resizeEvent(QResizeEvent *event)
{

            monitorFormW->p_graph->g_get_size(this->width(),this->height());


}




void frontend::fr_setTestLabel()
{


}




#include "frontend.moc"
