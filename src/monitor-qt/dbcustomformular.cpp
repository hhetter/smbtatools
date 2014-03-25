#include "dbcustomformular.h"
#include "ui_dbcustomformular.h"

dbcustomformular::dbcustomformular(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dbcustomformular)
{
    newshowformat = new showformat*[128];
    for(int i = 0; i < 128; i++)
    {
        newshowformat[i] = NULL;
    }
    graph = new MainWidget*[128];
    for(int i = 0; i < 128; i++)
    {
        graph[i] = NULL;
    }
    text_format = false;
    ui->setupUi(this);
    SELECT = "";
    ui->rBall->setChecked(1);
    ui->sBShowLimit->setEnabled(0);
    ui->cBRead->setChecked(1);
    ui->cBWrite->setChecked(1);
    ui->cBUsers->setChecked(1);
}

dbcustomformular::~dbcustomformular()
{
    delete ui;
}

void dbcustomformular::on_CancelButton_clicked()
{
    this->~dbcustomformular();
}

void dbcustomformular::on_StartButton_clicked()
{
    dbc_SQLQuery();
}

void dbcustomformular::dbc_SQLQuery()
{

    QSqlQuery q(dbc_build_query());
    rec = q.record();
    dbc_create_subwindow(q);
}

QString dbcustomformular::dbc_build_query()
{
    QString Query = "";
    QString LIMIT = "";
    QString ORDER = "";
    QString WHERE = "";


    if (ui->cBWrite->isChecked() && ui->cBRead->isChecked())
    {
        WHERE = " WHERE vfs_id = '0' or vfs_id = '2' ";
    }
    else if (ui->cBWrite->isChecked())
    {
        WHERE = " WHERE vfs_id = '2' ";
    }
    else
    {
        WHERE = " WHERE vfs_id = '0' ";
    }

    if (ui->cBUsers->isChecked())
    {
        SELECT = "username";
        Query = "SELECT username,share, COUNT(" + SELECT + ") from data " + WHERE + " GROUP BY username,share";
    }
    else if (ui->cBFiles->isChecked())
    {
        SELECT = "string1";
        Query = "SELECT string1,share, COUNT(" + SELECT + ") from data " + WHERE + " GROUP BY string1,share";
    }
    else if (ui->cBFilelength->isChecked())
    {
        SELECT = "length";
        Query = "SELECT string1,SUM(" + SELECT + ") from data " + WHERE + " GROUP BY string1";
    }
    else if(ui->cBShares->isChecked())
    {
        SELECT = "share";
        Query = "SELECT " + SELECT + ", COUNT(" + SELECT + ") from data " + WHERE + " GROUP BY " + SELECT;
    }
    if(!ui->rBall->isChecked() && ui->sBShowLimit->isEnabled())
    {
        LIMIT = " LIMIT " + ui->sBShowLimit->text();
    }
    if (ui->cBFilelength->isChecked() && ui->rBlowest->isChecked())
    {
        ORDER = " ORDER BY SUM(length) ASC ";
    }
    else if(ui->cBFilelength->isChecked() && ui->rBmost->isChecked())
    {
        ORDER = " ORDER BY SUM(length) DESC ";
    }
    else if(ui->rBlowest->isChecked())
    {
        ORDER = " ORDER BY COUNT(" + SELECT + ") ASC ";
    }
    else if(ui->rBmost->isChecked())
    {
        ORDER = " ORDER BY COUNT(" + SELECT + ") DESC ";
    }
    Query = Query + ORDER + LIMIT + ";";
    return Query;
}

void dbcustomformular::dbc_create_subwindow(QSqlQuery q)
{
    for(int i =0; i < 128; i++)
    {   if (text_format == true)
        {
            if(newshowformat[i] == NULL)
            {
                newshowformat[i] = new showformat();
                ui->mdiArea->addSubWindow(newshowformat[i]);
                newshowformat[i]->activateWindow();
                newshowformat[i]->show();
                ui->mdiArea->currentSubWindow()->resize( 360, 260 );
                int value1 = rec.indexOf(this->SELECT);
                int value2 = rec.indexOf("count");
                if (ui->cBFilelength->isChecked())
                {
                    value1 = rec.indexOf("string1");
                    int value2 = rec.indexOf("sum");
                    newshowformat[i]->new_Element(value1, value2, q);
                }
                else if (ui->cBFiles->isChecked())
                {
                    value1 = rec.indexOf("string1");
                    int value3 = rec.indexOf("share");
                    newshowformat[i]->new_Element(value1, value2, value3, q);
                }
                else if(ui->cBUsers->isChecked())
                {
                    value1 = rec.indexOf("username");
                    int value3 = rec.indexOf("share");
                    newshowformat[i]->new_Element(value1, value2, value3, q);
                }
                else
                {
                    newshowformat[i]->new_Element(value1, value2, q);
                }
                i=128;
            }
        }
        else
        {
            if(graph[i] == NULL)
            {
                graph[i] = new MainWidget();
                ui->mdiArea->addSubWindow(graph[i]);
                graph[i]->activateWindow();
                graph[i]->show();
                ui->mdiArea->currentSubWindow()->resize( 360, 260 );
                int value1;
                int value2 = rec.indexOf("count");
                int value3 = -1;
                if (ui->cBFilelength->isChecked())
                {
                    value1 = rec.indexOf("string1");
                    value2 = rec.indexOf("sum");
                    graph[i]->new_Element(value1, value2, value3, q);
                }
                else if (ui->cBFiles->isChecked())
                {
                    value1 = rec.indexOf("string1");
                    int value3 = rec.indexOf("share");
                    graph[i]->new_Element(value1, value2, value3, q);
                }
                else if(ui->cBUsers->isChecked())
                {
                    value1 = rec.indexOf("username");
                    int value3 = rec.indexOf("share");
                    graph[i]->new_Element(value1, value2, value3, q);
                }
                else
                {
                    value1 = rec.indexOf("share");
                    graph[i]->new_Element(value1, value2, q);
                }
                i=128;
            }
        }
    }
}

void dbcustomformular::set_format(bool format)
{
    text_format = format;
}

void dbcustomformular::on_rBmost_clicked()
{
    ui->sBShowLimit->setEnabled(1);
}

void dbcustomformular::on_rBlowest_clicked()
{
    ui->sBShowLimit->setEnabled(1);
}

void dbcustomformular::on_rBall_clicked()
{
    ui->sBShowLimit->setDisabled(1);
}

#include "dbcustomformular.moc"