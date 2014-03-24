#ifndef DBCUSTOMFORMULAR_H
#define DBCUSTOMFORMULAR_H

#include <QWidget>

namespace Ui {
class dbcustomformular;
}

class dbcustomformular : public QWidget
{
    Q_OBJECT

public:
    explicit dbcustomformular(QWidget *parent = 0);
    ~dbcustomformular();

private slots:
    void on_CancelButton_clicked();

    void on_StartButton_clicked();

    void on_cBShowLimit_clicked();

private:
    Ui::dbcustomformular *ui;    
};

#endif // DBCUSTOMFORMULAR_H
