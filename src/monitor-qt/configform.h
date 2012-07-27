#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <QWidget>
#include <QDebug>
#include <QComboBox>

namespace Ui {
    class ConfigForm;
}

class ConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigForm(QWidget *parent = 0);
    ~ConfigForm();

    QString *configString, *hostString, *portString, *shareString, *userString,
    *domainString, *fileString;
    int i_monitortime, i_intervaltime;






public slots:

    void cf_readconfig();
    void cf_hidefield();


private:
    Ui::ConfigForm *ui;
};

#endif // CONFIGFORM_H
