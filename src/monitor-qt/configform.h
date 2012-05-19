#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <QWidget>
#include <QDebug>

namespace Ui {
    class ConfigForm;
}

class ConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigForm(QWidget *parent = 0);
    ~ConfigForm();

    QString *configString;
    int i_monitortime, i_intervaltime;






public slots:

    void cf_readconfig();


private:
    Ui::ConfigForm *ui;
};

#endif // CONFIGFORM_H
