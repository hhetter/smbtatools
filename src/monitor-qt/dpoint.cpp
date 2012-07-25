#include "dpoint.h"

DPoint::DPoint(QObject *parent) :
    QObject(parent)
{
    i_s_count = 0;
    i_stepsize=5;
    i_max_index = 86400;

    for(int i = 0; i< 86400; i++){
        readlist.append(i);
        writelist.append(i);

    }

}


void DPoint::dp_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite){

//        qDebug() << "*l_fread: ->points " << *l_fread;
    if(i_s_count < i_stepsize )
    {
        l_read_end += *l_fread;
        l_write_end += *l_fwrite;
        i_s_count++;
    }
    if (i_s_count == i_stepsize)
    {
        l_read_end += *l_fread;
        l_write_end += *l_fwrite;

 //       qDebug() << "entervalue 1" << readlist[0];
        readlist.prepend(l_read_end);
//        qDebug() << "entervalue this " << l_read_end;
//        qDebug() << "entervalue 2" << readlist[0];
//        qDebug() << "================";
        writelist.prepend(l_write_end);

        if(readlist.size() == i_max_index+1 &&
                writelist.size() == i_max_index+1){
            readlist.removeLast();
            writelist.removeLast();

        }
        //qDebug()<<"readlist.size: "<< readlist.size();


        l_read_end = 0;
        l_read_end = 0;

        i_s_count = 0;

    }

}


QList<unsigned long> DPoint::dp_returnreadlist(){
    return readlist;
}



QList<unsigned long> DPoint::dp_returnwritelist(){
    return writelist;
}




#include "dpoint.moc"
