#include "pgdiagram.h"
#include "ui_pgdiagram.h"

pgdiagram::pgdiagram(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::pgdiagram)
{
    ui->setupUi(this);
}

pgdiagram::~pgdiagram()
{
    delete ui;
}

void pgdiagram::setDB(QSqlDatabase & db)
{
    this->db = db;
}

void pgdiagram::on_CloseButton_clicked()
{
    db.close();
    this->close();
}

void pgdiagram::on_actionMost_Users_triggered()
{

    QSqlQuery q("SELECT username,count(username),usersid FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY COUNT(username) DESC LIMIT 10;");
    QSqlRecord rec = q.record();

    int value1 = rec.indexOf("username");
    int value2 = rec.indexOf("usersid");
    int value3 = rec.indexOf("count");
    while (q.next())
    {
       qDebug() << q.value(value1).toString()
                << q.value(value2).toString()
                << q.value(value3).toString()
                ;
    }
}

void pgdiagram::on_actionLowest_Users_triggered()
{
    QSqlQuery q("SELECT username,count(username),usersid FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY username, usersid ORDER BY COUNT(username) ASC LIMIT 10;");
    QSqlRecord rec = q.record();

    int value1 = rec.indexOf("username");
    int value2 = rec.indexOf("usersid");
    int value3 = rec.indexOf("count");
    while (q.next())
    {
       qDebug() << q.value(value1).toString()
                << q.value(value2).toString()
                << q.value(value3).toString()
                ;
    }
}

void pgdiagram::on_actionMost_Files_triggered()
{
    QSqlQuery q("SELECT string1,count(string1),share FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY COUNT(string1) DESC LIMIT 10;");
    QSqlRecord rec = q.record();

    int value1 = rec.indexOf("string1");
    int value2 = rec.indexOf("share");
    int value3 = rec.indexOf("count");
    while (q.next())
    {
       qDebug() << q.value(value1).toString()
                << q.value(value2).toString()
                << q.value(value3).toString()
                ;
    }
}

void pgdiagram::on_actionLowest_Files_triggered()
{
    QSqlQuery q("SELECT string1,count(string1),share FROM data WHERE vfs_id = '0' OR vfs_id = '2' GROUP BY string1, share ORDER BY COUNT(string1) ASC LIMIT 10;");
    QSqlRecord rec = q.record();

    int value1 = rec.indexOf("string1");
    int value2 = rec.indexOf("share");
    int value3 = rec.indexOf("count");
    while (q.next())
    {
       qDebug() << q.value(value1).toString()
                << q.value(value2).toString()
                << q.value(value3).toString()
                ;
    }
}

void pgdiagram::on_actionCustom_triggered()
{
    dbcustom = new dbcustomformular();
    dbcustom->activateWindow();
    dbcustom->show();
}
