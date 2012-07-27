#include "processrunner.h"

Processrunner::Processrunner(QWidget *parent) :
    QObject(parent)
{
   //qDebug() << "Constructor Class Processrunner";

   monitorprocess = new QProcess;


}


void Processrunner::Processrunner::pr_readsignal(){

  //qDebug()<<"pr_readsignal  ";

}





#include "processrunner.moc"
