#include "databaseconnection.h"


databaseconnection::databaseconnection()
{

    db = QSqlDatabase::addDatabase("QPSQL");
}

void databaseconnection::connectto(const QString hostname, const QString database, const QString username, const QString passwd)
{
    db.setHostName(hostname);
    db.setDatabaseName(database);
    db.setUserName(username);
    db.setPassword(passwd);
    if(db.open())
    {
        pgd = new pgdiagram();
        pgd->setDB(db);
        pgd->activateWindow();
        pgd->show();
    }
}
