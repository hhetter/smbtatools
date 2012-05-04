#include "processrunner.h"

Processrunner::Processrunner(QWidget *parent) :
    QThread(parent)
{
   qDebug() << " Constructor Class Processrunner";
//           monitorprocess = new QProcess;

//   monitorprocess = new QProcess;
//   monitorprocess->start("./smbtamonitor-gen -h smbtad.de -i 3491 -g -I 0" );
//   monitorprocess->start("./owntools");
//   monitorprocess->start("./owntools3");
//   connect(monitorprocess, SIGNAL(readyReadStandardOutput()), qApp, SLOT(quit()));
//   connect(monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(pr_readsignal()));




}

void Processrunner::run(){



   qDebug() << "Processrunner::run";
       monitorprocess = new QProcess;
       monitorprocess->start("./smbtamonitor-gen -i 3491 -h 10.10.0.81 -u holger -I 0");







       //   monitorprocess->start("./smbtamonitor-gen -i 3491 -h 10.10.0.81 -u holger -I 0" );
//   monitorprocess->start("./owntools");

//   connect(monitorprocess, SIGNAL(readyReadStandardOutput()), qApp, SLOT(quit()));
   connect(monitorprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(pr_readsignal()));

   pr_readsignal();
   exec();

}


void Processrunner::Processrunner::pr_readsignal(){

  qDebug()<<"pr_readsignal  2";

}





#include "processrunner.moc"
