#ifndef DBCUSTOMFORMULAR_H
#define DBCUSTOMFORMULAR_H


#include "showformat.h"
#include "mainwidget.h"

namespace Ui {
class dbcustomformular;
}

class dbcustomformular : public QWidget
{
    Q_OBJECT

public:
    explicit dbcustomformular(QWidget *parent = 0);
    ~dbcustomformular();
    void set_format(bool);


private slots:
    void on_CancelButton_clicked();

    void on_StartButton_clicked();

    void on_rBmost_clicked();

    void on_rBlowest_clicked();

    void on_rBall_clicked();

private:
    MainWidget** graph;
    Ui::dbcustomformular *ui;    
    showformat** newshowformat;
    QString dbc_build_query();
    QString SELECT;
    QSqlRecord rec;
    bool text_format;
    void dbc_SQLQuery();
    void dbc_create_subwindow(QSqlQuery);


};

#endif // DBCUSTOMFORMULAR_H
