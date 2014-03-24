#ifndef PGFORMULAR_H
#define PGFORMULAR_H

#include "pgdiagram.h"

namespace Ui {
class pgformular;
}

class pgformular : public QWidget
{
    Q_OBJECT

public:
    explicit pgformular(QWidget *parent = 0);
    ~pgformular();

private slots:
    void on_ConnectButton_clicked();

    void on_ResetButton_clicked();

    void on_QuitButton_clicked();

private:
    Ui::pgformular *ui;
    QString qs_Hostname;
    QString qs_Databasename;
    QString qs_Username;
    QString qs_Passwd;
    QSqlDatabase qsdb;
    pgdiagram **pgd;
    void pgf_dbconnection();
    void pgf_opensubwindow();
};

#endif // PGFORMULAR_H
