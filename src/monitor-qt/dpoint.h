#ifndef DPOINT_H
#define DPOINT_H

#include <QObject>
#include <QList>
#include <QLinkedList>
#include <QPoint>
#include <QDebug>

#include "instancedata.h"

class DPoint : public QObject
{
    Q_OBJECT
public:
    explicit DPoint(InstanceData *idata, QObject *parent = 0);

//        unsigned long *l_fread, *l_fwrite;
        unsigned long l_read_end, l_write_end;
        int i_s_count;
        int i_stepsize;
        int i_max_index;

        QList<unsigned long> readlist, writelist;

signals:

public slots:


        void dp_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite);
        QList<unsigned long> dp_returnreadlist();
        QList<unsigned long> dp_returnwritelist();


};

#endif // DPOINT_H
