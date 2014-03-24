#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui>
#include <QtSql/QPSQLDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QString>
#include <QMdiSubWindow>
#include <QDebug>

namespace Ui {
    class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    void new_Element(int, int, QSqlQuery);
    void new_Element(int, int, int, QSqlQuery);
    ~MainWidget();

protected:
    void changeEvent(QEvent *e);

private:
    int *graphdata;
    int max;
    void paint();

    QSqlQuery q;
    QStringList data;
    Ui::MainWidget *ui;

    QStandardItemModel *itemModel;
};

#endif // MAINWIDGET_H
