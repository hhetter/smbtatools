#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QtSql/QPSQLDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <stdlib.h>
#include <QDebug>
#include <QtCore>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QString>
#include "pgdiagram.h"

namespace Ui {
class databaseconnection;
}


class databaseconnection
{
private:
    bool dbconn;
    QSqlDatabase db;
    pgdiagram *pgd;


public:
    databaseconnection();
    void connectto(const QString hostname, const QString database, const QString username, const QString passwd);
};

#endif // DATABASECONNECTION_H
