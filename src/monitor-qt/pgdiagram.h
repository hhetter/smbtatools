#ifndef PGDIAGRAM_H
#define PGDIAGRAM_H


#include "dbcustomformular.h"
#include "mainwidget.h"

namespace Ui {
class pgdiagram;
}

class pgdiagram : public QWidget
{
    Q_OBJECT

public:
    explicit pgdiagram(QWidget *parent = 0);
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

private:
    QString qst_query;
    QSqlRecord rec;
    dbcustomformular** dbcustom;
    Ui::pgdiagram *ui;
    showformat** newshowformat;
    MainWidget** graph;
    QSqlQuery q;
    QString value;
    void pgd_SQLQuery(QString value);
};


#endif // PGDIAGRAM_H
