#ifndef PGDIAGRAM_H
#define PGDIAGRAM_H


#include "dbcustomformular.h"
#include "mainwidget.h"
#include "frontend.h"
#include "instancedata.h"

namespace Ui {
class pgdiagram;
}

class pgdiagram : public QWidget
{
    Q_OBJECT

public:
    explicit pgdiagram(InstanceData *idata, QWidget *parent = 0);
    void set_dbconnection(bool connection);
    ~pgdiagram();

private slots:

    void on_Quit_triggered();

    void on_Dia_m_10_U_triggered();

    void on_Tex_m_10_U_triggered();

    void on_Dia_l_10_U_triggered();

    void on_Tex_l_10_U_triggered();

    void on_Dia_m_10_F_triggered();

    void on_Tex_m_10_F_triggered();

    void on_Dia_l_10_F_triggered();

    void on_Tex_l_10_F_triggered();

    void on_Dia_Custom_triggered();

    void on_Tex_Custom_triggered();

    void on_actionLiveanalyzer_triggered();


private:
    InstanceData *ldata;
    frontend *newFrontendArray[128];
    //frontend *newFrontend;
    QString qst_query;
    QSqlRecord rec;
    Ui::pgdiagram *ui;
    QSqlQuery q;
    QString value;
    QSqlDatabase qsdb;
    bool dbconnection;
    MainWidget *graph[128];
    showformat *newshowformat[128];
    dbcustomformular *dbcustom[128];



    void pgd_SQLQuery(QString value);
};


#endif // PGDIAGRAM_H
