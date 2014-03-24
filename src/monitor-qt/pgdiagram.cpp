#include "pgdiagram.h"
#include "ui_pgdiagram.h"

pgdiagram::pgdiagram(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pgdiagram)
{
    newshowformat = new showformat*[128];
    for(int x = 0; x < 128; x++)
    {
        this->newshowformat[x] = NULL;
    }
    dbcustom = new dbcustomformular*[128];
    for(int x = 0; x < 128; x++)
    {
        this->dbcustom[x] = NULL;
    }
    graph = new MainWidget*[128];
    for(int i = 0; i < 128; i++)
    {
        graph[i] = NULL;
    }
    ui->setupUi(this);
}

pgdiagram::~pgdiagram()
{
    delete ui;
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
    this->qst_query = "SELECT username,count(username) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY COUNT(username) DESC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_m_10_U_triggered()
{
    this->qst_query = "SELECT username,count(username) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY COUNT(username) DESC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
            newshowformat[i]->new_Element(value1, value2, q);
            i=128;
        }
    }
}

void pgdiagram::on_Dia_l_10_U_triggered()
{
    this->qst_query = "SELECT username,count(username) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY COUNT(username) ASC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_l_10_U_triggered()
{
    this->qst_query = "SELECT username,count(username) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY COUNT(username) ASC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
            newshowformat[i]->new_Element(value1, value2, q);
            i=128;
        }
    }
}

void pgdiagram::on_Dia_m_10_F_triggered()
{
    this->qst_query = "SELECT string1,count(string1) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY COUNT(string1) DESC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_m_10_F_triggered()
{
    this->qst_query = "SELECT string1,count(string1) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY COUNT(string1) DESC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
            newshowformat[i]->new_Element(value1, value2, q);
            i=128;
        }
    }
}

void pgdiagram::on_Dia_l_10_F_triggered()
{
    this->qst_query = "SELECT string1,count(string1) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY COUNT(string1) ASC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
            graph[i]->new_Element(value1, value2, q);
            i = 128;
        }
    }
}

void pgdiagram::on_Tex_l_10_F_triggered()
{
    this->qst_query = "SELECT string1,count(string1) FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY COUNT(string1) ASC LIMIT 10;";
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
            int value2 = this->rec.indexOf("count");
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
            dbcustom[i] = new dbcustomformular();
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
            dbcustom[i] = new dbcustomformular();
            ui->mdiADia->addSubWindow(dbcustom[i]);
            dbcustom[i]->activateWindow();
            dbcustom[i]->show();
            dbcustom[i]->set_format(true);
            ui->mdiADia->currentSubWindow()->resize(380,430);
            i=128;
        }
    }
}

#include "pgdiagram.moc"
