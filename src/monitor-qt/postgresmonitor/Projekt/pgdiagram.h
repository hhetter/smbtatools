#ifndef PGDIAGRAM_H
#define PGDIAGRAM_H

#include <QMainWindow>
#include <QtSql/QPSQLDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <stdlib.h>
#include <QDebug>
#include <QtCore>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QString>
#include "dbcustomformular.h"

namespace Ui {
class pgdiagram;
}

class pgdiagram : public QMainWindow
{
    Q_OBJECT

public:
    explicit pgdiagram(QWidget *parent = 0);
    void setDB(QSqlDatabase & db);
    ~pgdiagram();

private slots:

    void on_CloseButton_clicked();

    void on_actionMost_Users_triggered();

    void on_actionLowest_Users_triggered();

    void on_actionMost_Files_triggered();

    void on_actionLowest_Files_triggered();

    void on_actionCustom_triggered();

private:
    dbcustomformular* dbcustom;
    Ui::pgdiagram *ui;
    QSqlDatabase db;
};

#endif // PGDIAGRAM_H
