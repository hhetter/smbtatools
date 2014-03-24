#include "pgformular.h"
#include "ui_pgformular.h"

pgformular::pgformular(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::pgformular)
{
    dbcon = databaseconnection();
    ui->setupUi(this);
}

pgformular::~pgformular()
{
    delete ui;
}

void pgformular::on_ConnectButton_clicked()
{
    this->Hostname = ui->ServerLine->text();
    this->Databasename = ui->DatabaseLine->text();
    this->Username = ui->UsernameLine->text();
    this->Passwd = ui->PassLine->text();
    dbcon.connectto(this->Hostname, this->Databasename, this->Username, this->Passwd);
}

void pgformular::on_ResetButton_clicked()
{
    ui->ServerLine->clear();
    ui->DatabaseLine->clear();
    ui->UsernameLine->clear();
    ui->PassLine->clear();
}

void pgformular::on_QuitButtont_clicked()
{
    qApp->quit();
}
