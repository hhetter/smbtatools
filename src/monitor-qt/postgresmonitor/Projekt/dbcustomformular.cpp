#include "dbcustomformular.h"
#include "ui_dbcustomformular.h"
#include <QString>
#include <QDebug>

dbcustomformular::dbcustomformular(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dbcustomformular)
{
    ui->setupUi(this);
    ui->cBShowLimit->setChecked(1);
    ui->sBShowLimit->setEnabled(1);
}

dbcustomformular::~dbcustomformular()
{
    delete ui;
}

void dbcustomformular::on_CancelButton_clicked()
{
    this->close();
}

void dbcustomformular::on_StartButton_clicked()
{
    QString Query = "";
    QString LIMIT = "";
    QString ORDER = "";
    QString WHERE = "";
    QString SELECT = "";
    if(ui->cBShowLimit->isChecked())
    {
        LIMIT = "LIMIT " + ui->sBShowLimit->text();
    }

    if (ui->cBWrite->isChecked() && ui->cBRead->isChecked())
    {
        WHERE = "WHERE vfs_id = '0' or vfs_id = '2' ";
    }
    else if (ui->cBWrite->isChecked())
    {
        WHERE = "WHERE vfs_id = '2' ";
    }
    else
    {
        WHERE = "WHERE vfs_id = '0' ";
    }
    if (ui->cBUsers->isChecked())
    {
        SELECT = "username,usersid ";
    }

    if(ui->rBlowest->isChecked())
    {
        ORDER = "ORDER BY " + SELECT + "ASC ";
    }
    else if(ui->rBmost->isChecked())
    {
        ORDER = "ORDER BY " + SELECT + "DESC ";
    }
    Query = "SELECT " + SELECT + WHERE + "GROUP BY " + SELECT + ORDER + LIMIT + ";";
    qDebug() << Query;



}

void dbcustomformular::on_cBShowLimit_clicked()
{
    if(!ui->cBShowLimit->isChecked())
    {
        ui->sBShowLimit->setDisabled(1);
    }
    else
    {
        ui->sBShowLimit->setEnabled(1);
    }
}
