#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

#include <QObject>
#include <QList>
#include <QLinkedList>
#include <QPoint>
#include <QDebug>
#include "../../include/version.h"

class InstanceData : public QObject
{
    Q_OBJECT
public:
    explicit InstanceData(QObject *parent = 0);

        QString configString, hostString, portString, shareString, userString,
            domainString, fileString;
        QString nameString;
        QString queryParameterString;
        QString qs_Hostname;
        QString qs_Databasename;
        QString qs_Username;
        QString qs_Passwd;
        QString qs_Type;
        QString pidString;
        QString socketString;
        bool socketConnected;
        int i_monitortime, i_intervaltime;
        bool config_open;
        bool idCheck, dryrunCheck;
        
        int i_stepsize;
        int i_max_index;
    
        QList<unsigned long> readlist, writelist;

        QString licenseString;

signals:

public slots:



};

#endif // INSTANCEDATA_H
