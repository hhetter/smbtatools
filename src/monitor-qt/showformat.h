#ifndef SHOWFORMAT_H
#define SHOWFORMAT_H

#include <QWidget>
#include <QtSql/QPSQLDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QString>
#include <QMdiSubWindow>

namespace Ui {
class showformat;
}

class showformat : public QWidget
{
    Q_OBJECT

public:
    void new_Element(int, int, QSqlQuery);
    void new_Element(int, int, int, QSqlQuery);
    explicit showformat(QWidget *parent = 0);
    ~showformat();

private:
    Ui::showformat *ui;
};

#endif // SHOWFORMAT_H
