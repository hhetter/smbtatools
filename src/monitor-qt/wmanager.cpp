#include "wmanager.h"
#include "ui_wmanager.h"

WManager::WManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WManager)
{
    ui->setupUi(this);

    connect(ui->queryButton, SIGNAL(clicked()),this,
            SLOT(wm_newMonitorWidget()));
    connect(ui->quitButton, SIGNAL(clicked()),qApp,
            SLOT(quit()));




}

WManager::~WManager()
{
    delete ui;
}

void WManager::wm_newMonitorWidget()
{


    qDebug()<<"wm_newMonitorWidget()";
    //    ui->testlabel->setText("setTestLabel()");


}


#include "wmanager.moc"
