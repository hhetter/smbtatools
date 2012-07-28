#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <QWidget>
#include <QDebug>
#include <QComboBox>

#include "instancedata.h"

namespace Ui {
    class ConfigForm;
}

class ConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigForm(InstanceData *idata, QWidget *parent = 0);
    ~ConfigForm();

    QString *configString, *hostString, *portString, *shareString, *userString,
    *domainString, *fileString;
    int i_monitortime, i_intervaltime;






public slots:

    void cf_readconfig();
    void cf_hidefield();
    void cf_hideall();


private:
    Ui::ConfigForm *ui;
};

#endif // CONFIGFORM_H
