#include "frontend.h"
#include "ui_frontend.h"


frontend::frontend(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frontend)
{
    ui->setupUi(this);

    ////
    // Create the Widgets for configuration and the monitoring

    configWidget = new ConfigForm;
    monitorFormW = new MonitorForm();


    ////
    // Add widgets to the stackedWidget

    ui->stackedView->addWidget(configWidget);
    ui->stackedView->addWidget(monitorFormW);
    ui->stackedView->show();
    ui->stackedView->setCurrentIndex(0);


    connect(ui->configbutton, SIGNAL(clicked()),this, SLOT(fr_setConfigWidget()));
    connect(ui->monitorbutton, SIGNAL(clicked()),this, SLOT(fr_setMonitorWidget()));
    connect(ui->quitbutton, SIGNAL(clicked()),qApp, SLOT(quit()));


}

frontend::~frontend()
{
    delete ui;
}

void frontend::fr_setConfigWidget()
{


    qDebug()<<"fr_setConfigWidget()";
    ui->stackedView->setCurrentIndex(3);
    ui->testlabel->setText("fr_setConfigWidget()");


}


void frontend::fr_setMonitorWidget()
{

    configWidget->cf_readconfig();

    monitorFormW->configString->append(*configWidget->configString);
    monitorFormW->i_monitortime = configWidget->i_monitortime;
    monitorFormW->i_intervaltime = configWidget->i_intervaltime;

    qDebug()<<"fr_setMonitorWidget()";
    ui->stackedView->setCurrentIndex(4);
    ui->testlabel->setText("fr_setMonitorWidget()");


}




void frontend::fr_setTestLabel()
{


    qDebug()<<"setTestLabel()";
    ui->testlabel->setText("setTestLabel()");


}


#include "frontend.moc"
