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

    wm_firstInit();




}

WManager::~WManager()
{
    delete ui;
}

void WManager::wm_newMonitorWidget()
{


    qDebug()<<"wm_newMonitorWidget()";


    newFrontend = new frontend(this);
    newFrontend->show();

    //    ui->testlabel->setText("setTestLabel()");
        /*
    scene = new QGraphicsScene();
    QGraphicsRectItem *rect = scene->addRect(QRectF(0,0,100,100));
    QGraphicsItem *item = scene->itemAt(50,50);
    QGraphicsView view(scene);
    view.show();
 //   scene->addItem(newFrontend);
        */
    QGraphicsScene scene;
    scene.addWidget(newFrontend);
    scene.setSceneRect( -100.0, -100.0, 200.0, 200.0 );

    QGraphicsEllipseItem *item = new QGraphicsEllipseItem( 0, &scene );
    item->setRect( -50.0, -50.0, 100.0, 100.0 );

    QGraphicsView view( &scene );
    view.setRenderHints( QPainter::Antialiasing );

    ui->wmLayout->addWidget(&view);
    view.show();

    QLabel aLabel("Text");
    ui->wmLayout->addWidget(&aLabel);
    ui->wmLayout->setLayout();
    aLabel.show();




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





#include "wmanager.moc"
