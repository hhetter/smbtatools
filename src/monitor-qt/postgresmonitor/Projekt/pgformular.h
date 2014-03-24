#ifndef PGFORMULAR_H
#define PGFORMULAR_H

#include <QMainWindow>
#include "databaseconnection.h"

namespace Ui {
class pgformular;
}

class pgformular : public QMainWindow
{
    Q_OBJECT

public:
    explicit pgformular(QWidget *parent = 0);
    ~pgformular();

private slots:
    void on_ConnectButton_clicked();

    void on_ResetButton_clicked();

    void on_QuitButtont_clicked();

private:
    Ui::pgformular *ui;
    databaseconnection dbcon;
    QString Hostname;
    QString Databasename;
    QString Username;
    QString Passwd;
};

#endif // PGFORMULAR_H
