#include "pgformular.h"
#include "ui_pgformular.h"

pgformular::pgformular(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pgformular)
{
    pgd = new pgdiagram*[128];
    for(int x = 0; x < 128; x++)
    {
        this->pgd[x] = NULL;
    }
    this->ui->setupUi(this);
}

pgformular::~pgformular()
{
    delete ui;
}

void pgformular::on_ConnectButton_clicked()
{
    this->pgf_dbconnection();
    this->pgf_opensubwindow();
}

void pgformular::on_ResetButton_clicked()
{
    this->ui->ServerLine->clear();
    this->ui->DatabaseLine->clear();
    this->ui->UsernameLine->clear();
    this->ui->PassLine->clear();
}

void pgformular::on_QuitButton_clicked()
{
    qApp->quit();
}

void pgformular::pgf_dbconnection()
{
    this->qsdb = QSqlDatabase::addDatabase("QPSQL"); //Set Database Type to Postgres
    //Set the Login datas
    this->qs_Hostname = ui->ServerLine->text();
    this->qs_Databasename = ui->DatabaseLine->text();
    this->qs_Username = ui->UsernameLine->text();
    this->qs_Passwd = ui->PassLine->text();

    this->qsdb.setHostName(qs_Hostname);
    this->qsdb.setDatabaseName(qs_Databasename);
    this->qsdb.setUserName(qs_Username);
    this->qsdb.setPassword(qs_Passwd);
}

void pgformular::pgf_opensubwindow()
{
    //open database connection
    if(this->qsdb.open())
    {
        for(int i =0; i < 128; i++)
        {
            if(pgd[i] == NULL)
            {
                pgd[i] = new pgdiagram();
                ui->mdiArea->addSubWindow(pgd[i]);
                pgd[i]->activateWindow();
                pgd[i]->show();
              //  ui->mdiArea->currentSubWindow()->resize( 780, 500 );
                i=128;
            }
        }
    }
}

/*void pgformular::resizeEvent(QResizeEvent *event)
{

     //       monitorFormW->p_graph->g_get_size(this->width(),this->height());


}

*/


#include "pgformular.moc"
