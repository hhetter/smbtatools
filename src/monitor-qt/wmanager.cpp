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

        for(int i=0; i < 128; i++){
            newFrontendArray[i] = 0;
        }


        wm_firstInit();




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


        qDebug()<<"wm_newMonitorWidget()";

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

void WManager::resizeEvent(QResizeEvent *event)
{
         // qDebug()<<"WManager Resized";

}




#include "wmanager.moc"
