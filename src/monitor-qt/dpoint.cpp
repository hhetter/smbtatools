#include "dpoint.h"

DPoint::DPoint(QObject *parent) :
    QObject(parent)
{
        i_s_count = 0;
        i_stepsize=5;
        i_max_index = 86400;

}


void DPoint::dp_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite){

        if(i_s_count < i_stepsize-1 )
        {
                l_read_end += *l_fread;
                l_write_end += *l_fwrite;
                i_s_count++;
        }
        if (i_s_count == i_stepsize-1)
        {
                l_read_end += *l_fread;
                l_write_end += *l_fwrite;

                readlist.prepend(l_read_end);
                writelist.prepend(l_write_end);

                if(readlist.size() == i_max_index &&
                                writelist.size() == i_max_index){
                        readlist.removeLast();
                        writelist.removeLast();
                }
                qDebug()<<"readlist.size: "<< readlist.size();


                l_read_end = 0;
                l_read_end = 0;

                i_s_count = 0;

        }

}


#include "dpoint.moc"
