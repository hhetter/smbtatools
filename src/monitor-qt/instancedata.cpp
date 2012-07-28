#include "instancedata.h"

InstanceData::InstanceData(QObject *parent) :
        QObject(parent)
{
    i_stepsize = 5;
        

}


void InstanceData::dp_receivenumbers(unsigned long *l_fread, unsigned long *l_fwrite){
        

       

}


QList<unsigned long> InstanceData::dp_returnreadlist(){
        return readlist;
}



QList<unsigned long> InstanceData::dp_returnwritelist(){
        return writelist;
}




#include "instancedata.moc"
