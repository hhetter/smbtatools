#include "pgdiagram.h"
#include "ui_pgdiagram.h"

pgdiagram::pgdiagram(InstanceData *idata, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pgdiagram)
{
    ldata = idata;
    QString db_type = ldata->qs_Type;
    qsdb = QSqlDatabase::addDatabase(db_type); //Set Database Type to Postgres

    qsdb.setHostName(ldata->qs_Hostname);
    qsdb.setDatabaseName(ldata->qs_Databasename);
    qsdb.setUserName(ldata->qs_Username);
    qsdb.setPassword(ldata->qs_Passwd);
    for(int i=0; i < 128; i++){
        newFrontendArray[i] = 0;
        newshowformat[i] = NULL;
        graph[i] = NULL;
        dbcustom[i] = NULL;
    }
    ui->setupUi(this);
    if(this->qsdb.open())
    {
        set_dbconnection(true);
    }
    else
    {
        set_dbconnection(false);
    }
}

pgdiagram::~pgdiagram()
{
    delete ui;
}

void pgdiagram::set_dbconnection(bool connection)
{
    dbconnection = connection;
    if(dbconnection == false)
    {
        ui->menuCustom->setDisabled(1);
        ui->menuLowest_10_Files->setDisabled(1);
        ui->menuLowest_10_Users->setDisabled(1);
        ui->menuMost_10_Files->setDisabled(1);
        ui->menuMost_10_Users->setDisabled(1);
    }
}

void pgdiagram::pgd_SQLQuery(QString value)
{
    QSqlQuery q(this->qst_query);
    this->rec = q.record();
    this->value = value;
    this->q = q;
}

void pgdiagram::on_Quit_triggered()
{
    this->~pgdiagram();
}

void pgdiagram::on_Dia_m_10_U_triggered()
{
    this->qst_query = "SELECT username,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY SUM(length) DESC LIMIT 10;";
    this->pgd_SQLQuery("username");
    for(int i = 0; i < 128; i++)
    {
        if(graph[i] == NULL)
        {
            graph[i] = new MainWidget();
            ui->mdiADia->addSubWindow(graph[i]);
            graph[i]->activateWindow();
            graph[i]->show();
            ui->mdiADia->currentSubWindow()->resize(380,430);
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_m_10_U_triggered()
{
    this->qst_query = "SELECT username,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY SUM(length) DESC LIMIT 10;";
    this->pgd_SQLQuery("username");
    for(int i =0; i < 128; i++)
    {
        if(newshowformat[i] == NULL)
        {
            newshowformat[i] = new showformat();
            ui->mdiADia->addSubWindow(newshowformat[i]);
            newshowformat[i]->activateWindow();
            newshowformat[i]->show();
            ui->mdiADia->currentSubWindow()->resize( 380, 430 );
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            newshowformat[i]->new_Element(value1, value2, q);
            i=128;
        }
    }
}

void pgdiagram::on_Dia_l_10_U_triggered()
{
    this->qst_query = "SELECT username,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY SUM(length) ASC LIMIT 10;";
    this->pgd_SQLQuery("username");
    for(int i = 0; i < 128; i++)
    {
        if(graph[i] == NULL)
        {
            graph[i] = new MainWidget();
            ui->mdiADia->addSubWindow(graph[i]);
            graph[i]->activateWindow();
            graph[i]->show();
            ui->mdiADia->currentSubWindow()->resize(380,430);
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_l_10_U_triggered()
{
    this->qst_query = "SELECT username,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY SUM(length) ASC LIMIT 10;";
    this->pgd_SQLQuery("username");
    for(int i =0; i < 128; i++)
    {
        if(newshowformat[i] == NULL)
        {
            newshowformat[i] = new showformat();
            ui->mdiADia->addSubWindow(newshowformat[i]);
            newshowformat[i]->activateWindow();
            newshowformat[i]->show();
            ui->mdiADia->currentSubWindow()->resize( 380, 430 );
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            newshowformat[i]->new_Element(value1, value2, q);
            i=128;
        }
    }
}

void pgdiagram::on_Dia_m_10_F_triggered()
{
    this->qst_query = "SELECT string1,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY SUM(length) DESC LIMIT 10;";
    this->pgd_SQLQuery("string1");
    for(int i = 0; i < 128; i++)
    {
        if(graph[i] == NULL)
        {
            graph[i] = new MainWidget();
            ui->mdiADia->addSubWindow(graph[i]);
            graph[i]->activateWindow();
            graph[i]->show();
            ui->mdiADia->currentSubWindow()->resize(380,430);
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_m_10_F_triggered()
{
    this->qst_query = "SELECT string1,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY SUM(length) DESC LIMIT 10;";
    this->pgd_SQLQuery("string1");
    for(int i =0; i < 128; i++)
    {
        if(newshowformat[i] == NULL)
        {
            newshowformat[i] = new showformat();
            ui->mdiADia->addSubWindow(newshowformat[i]);
            newshowformat[i]->activateWindow();
            newshowformat[i]->show();
            ui->mdiADia->currentSubWindow()->resize( 380, 430 );
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            newshowformat[i]->new_Element(value1, value2, q);
            i=128;
        }
    }
}

void pgdiagram::on_Dia_l_10_F_triggered()
{
    this->qst_query = "SELECT string1,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY SUM(length) ASC LIMIT 10;";
    this->pgd_SQLQuery("string1");
    for(int i = 0; i < 128; i++)
    {
        if(graph[i] == NULL)
        {
            graph[i] = new MainWidget();
            ui->mdiADia->addSubWindow(graph[i]);
            graph[i]->activateWindow();
            graph[i]->show();
            ui->mdiADia->currentSubWindow()->resize(380,430);
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_l_10_F_triggered()
{
    this->qst_query = "SELECT string1,sum(length) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY SUM(length) ASC LIMIT 10;";
    this->pgd_SQLQuery("string1");
    for(int i =0; i < 128; i++)
    {
        if(newshowformat[i] == NULL)
        {
            newshowformat[i] = new showformat();
            ui->mdiADia->addSubWindow(newshowformat[i]);
            newshowformat[i]->activateWindow();
            newshowformat[i]->show();
            ui->mdiADia->currentSubWindow()->resize( 380, 430 );
            int value1 = this->rec.indexOf(value);
            int value2 = this->rec.indexOf("sum");
            newshowformat[i]->new_Element(value1, value2, q);
            i=128;
        }
    }
}

void pgdiagram::on_Dia_Custom_triggered()
{
    for(int i =0; i < 128; i++)
    {
        if(dbcustom[i] == NULL)
        {
            dbcustom[i] = new dbcustomformular(newshowformat, graph);
            ui->mdiADia->addSubWindow(dbcustom[i]);
            dbcustom[i]->activateWindow();
            dbcustom[i]->show();
            dbcustom[i]->set_format(false);
            ui->mdiADia->currentSubWindow()->resize(380,430);
            i=128;
        }
    }
}

void pgdiagram::on_Tex_Custom_triggered()
{
    for(int i =0; i < 128; i++)
    {
        if(dbcustom[i] == NULL)
        {
            dbcustom[i] = new dbcustomformular(newshowformat, graph);
            ui->mdiADia->addSubWindow(dbcustom[i]);
            dbcustom[i]->activateWindow();
            dbcustom[i]->show();
            dbcustom[i]->set_format(true);
            ui->mdiADia->currentSubWindow()->resize(380,430);
            i=128;
        }
    }
}

void pgdiagram::on_actionLiveanalyzer_triggered()
{
    for(int i =0; i < 128; i++){
            if(newFrontendArray[i] == 0){
                newFrontendArray[i] = new frontend(ldata);
                ui->mdiADia->addSubWindow(newFrontendArray[i]);
                newFrontendArray[i]->activateWindow();
                newFrontendArray[i]->show();
                ui->mdiADia->currentSubWindow()->resize( 670, 540 );
                //newFrontendArray[i]->resize(670,540);
                i=128;
            }

        }
}


#include "pgdiagram.moc"
