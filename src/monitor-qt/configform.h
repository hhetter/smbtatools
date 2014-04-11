#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <QWidget>
#include <QDebug>
#include <QComboBox>
#include <QtSql/QPSQLDriver>
#include <QtSql/QSqlDatabase>


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

        InstanceData *ldata;

        QString configString, hostString, portString, shareString, userString,
        domainString, fileString;
        int i_monitortime, i_intervaltime;






public slots:
        void cf_saveconfig();
        void cf_readconfig();
        void cf_hidefield();
        void cf_hideall();


private:
        Ui::ConfigForm *ui;
        QString qs_Hostname;
        QString qs_Databasename;
        QString qs_Username;
        QString qs_Passwd;
        bool dbconnection;
};

#endif // CONFIGFORM_H
