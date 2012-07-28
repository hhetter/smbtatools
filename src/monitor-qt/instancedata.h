#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

#include <QObject>
#include <QList>
#include <QLinkedList>
#include <QPoint>
#include <QDebug>

class InstanceData : public QObject
{
    Q_OBJECT
public:
    explicit InstanceData(QObject *parent = 0);

        QString configString;
        QString nameString, fileString, domainString, shareString, userString;
        QString portString, hostString;
        
        int i_stepsize;
    
        QList<unsigned long> readlist, writelist;

signals:

public slots:


        void dp_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite);
        QList<unsigned long> dp_returnreadlist();
        QList<unsigned long> dp_returnwritelist();


};

#endif // INSTANCEDATA_H
