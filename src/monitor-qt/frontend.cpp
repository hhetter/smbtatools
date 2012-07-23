#include "frontend.h"
#include "ui_frontend.h"


frontend::frontend(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::frontend)
{
        ui->setupUi(this);
	this->setWindowTitle("smbtamonitor-qt");
        ////
        // Create the Widgets for configuration and the monitoring

        configWidget = new ConfigForm();
        monitorFormW = new MonitorForm();


        ////
        // Add widgets to the stackedWidget

        ui->stackedView->addWidget(configWidget);
        ui->stackedView->addWidget(monitorFormW);
        ui->stackedView->show();
        ui->stackedView->setCurrentIndex(0);


        connect(ui->configbutton, SIGNAL(clicked()),this,
                SLOT(fr_setConfigWidget()));
        connect(ui->monitorbutton, SIGNAL(clicked()),this,
                SLOT(fr_setMonitorWidget()));
        connect(ui->quitbutton, SIGNAL(clicked()),qApp,
                SLOT(quit()));

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


void frontend::fr_setConfigWidget()
{
        ui->stackedView->setCurrentIndex(3);
}


void frontend::fr_setMonitorWidget()
{       
        configWidget->cf_readconfig();

        monitorFormW->configString->append(*configWidget->configString);
        monitorFormW->i_monitortime = configWidget->i_monitortime;
        monitorFormW->i_intervaltime = configWidget->i_intervaltime;

        ui->stackedView->setCurrentIndex(4);
        //    ui->testlabel->setText("fr_setMonitorWidget()");
}

void frontend::resizeEvent(QResizeEvent *event)
{
        //    qDebug()<<"Frontend Resized";

}




void frontend::fr_setTestLabel()
{


        qDebug()<<"setTestLabel()";
        //    ui->testlabel->setText("setTestLabel()");


}




#include "frontend.moc"
