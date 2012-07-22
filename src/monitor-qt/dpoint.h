#ifndef DPOINT_H
#define DPOINT_H

#include <QObject>
#include <QLinkedList>
#include <QPoint>
#include <QDebug>

class DPoint : public QObject
{
    Q_OBJECT
public:
    explicit DPoint(QObject *parent = 0);

//        unsigned long *l_fread, *l_fwrite;
        unsigned long l_read_end, l_write_end;
        int i_s_count;
        int i_stepsize;
        int i_max_index;

        QLinkedList<unsigned long> readlist, writelist;

signals:

public slots:


        void dp_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite);
        QLinkedList<unsigned long> dp_returnreadlist();
        QLinkedList<unsigned long> dp_returnwritelist();


};

#endif // DPOINT_H
