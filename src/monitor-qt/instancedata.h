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

        QString configString;
        QString nameString, fileString, domainString, shareString, userString;
        QString queryParameterString;
        QString portString, hostString;

        QString pidString;
        QString socketString;
        bool socketConnected;

        bool idCheck, dryrunCheck;
        
        int i_stepsize;
        int i_max_index;
    
        QList<unsigned long> readlist, writelist;

        QString licenseString;

signals:

public slots:



};

#endif // INSTANCEDATA_H
