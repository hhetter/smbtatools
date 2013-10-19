#include "wmanager.h"
#include "ui_wmanager.h"

WManager::WManager(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::WManager)
{

        ui->setupUi(this);

        s_path = QDir::homePath().append("/.smbtatools");
        cf_path = s_path+"/smbtamonitor-qt.conf";
        df_path = s_path+"/smbtamonitor-qt.db";


        connect(ui->queryButton, SIGNAL(clicked()),this,
                SLOT(wm_newMonitorWidget()));
        connect(ui->quitButton, SIGNAL(clicked()),qApp,
                SLOT(quit()));

        for(int i=0; i < 128; i++){
            newFrontendArray[i] = 0;
        }


        wm_firstInit();
        wm_init();




}

WManager::~WManager()
{
        delete ui;
}

void WManager::wm_newMonitorWidget()
{

    for(int i =0; i < 128; i++){
        if(newFrontendArray[i] == 0){
            newFrontendArray[i] = new frontend();
            ui->mdiArea->addSubWindow(newFrontendArray[i]);
            newFrontendArray[i]->activateWindow();
            newFrontendArray[i]->show();
            ui->mdiArea->currentSubWindow()->resize( 670, 540 );
            //newFrontendArray[i]->resize(670,540);
            i=128;
        }

    }



    /*
        newFrontend = new frontend();
        newFrontend->show();

        //mdiArea
        ui->mdiArea->addSubWindow(newFrontend);
        newFrontend->activateWindow();
        newFrontend->show();
*/




}



void WManager::wm_firstInit(){
        /*
        *  Test if ~/.smbtatools exists - if not create it
        *  ~/.smbtatools will store configuration files and stuff
        */


        if(!QDir(s_path).exists())
        {
                QDir().mkdir(s_path);
                qDebug()<<"Initial setup: .smbtatools home directory created.";
        }
        if(QDir(s_path).exists() &&
                        !(QFile(cf_path).exists()))
        {

                QFile qf_conf_file(cf_path);

                qf_conf_file.open(QIODevice::WriteOnly | QIODevice::Text);
                QTextStream init(&qf_conf_file);
                init << "Smbtamonitor-qt configuration file\n";
                qf_conf_file.close();
                qDebug()<<"Smbtamonitor-qt config file created:";
                qDebug()<< cf_path;
        }

        /*
         * Test if sqlite db exists - if not, create one
         */

        /*
         *Debugging
        qDebug()<<"s_path: "<<s_path;
        if(QDir(s_path).exists()){qDebug()<<"s_path exist test succeeded";}
        qDebug()<<"df_path: "<<df_path;
        if(!(QFile(df_path).exists())){qDebug()<<"s_path doesn't exist test succeeded";}
        */
        if(QDir(s_path).exists() &&
                        !(QFile(df_path).exists()))
        {
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(df_path);
            db.open();
            // Setup database
            qDebug()<<"Setup database";
            db.exec("create table query_info "
                    "(id integer primary key, "
                    "nameString varchar(256), "
                    "fileString varchar(256), "
                    "domainString varchar(256), "
                    "shareString varchar(256), "
                    "userString varchar(256), "
                    "queryParameterString varchar(256), "
                    "portString varchar(256), "
                    "hostString varchar(256), "
                    );


            qDebug()<<"Smbtamonitor-qt database created:";
            qDebug()<< df_path;
            db.close();
        }
}

void WManager::wm_init(){

    // Sanity check
    if( !(QDir(s_path).exists()))
    {
        qDebug() << "Configuration directory: " << s_path;
        qDebug()<< "Sanity check: Failed to locate configuration directory - exiting";
        qApp->quit();
    }
    if( !(QFile(cf_path).exists()) )
    {
        qDebug() << "Configuration file: " << cf_path;
        qDebug()<< "Sanity check: Failed to locate configuration file - exiting";
        qApp->quit();
    }
    if( !(QFile(df_path).exists()) )
    {
        qDebug() << "Database: " << df_path;
        qDebug()<< "Sanity check: Failed to locate database file - exiting";
        qApp->quit();
    }

    // Open connection to database
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(df_path);
        db.open();

    // Populate smbtamonitor-qt


    db.close();


}


void WManager::resizeEvent(QResizeEvent *event)
{


}




#include "wmanager.moc"
