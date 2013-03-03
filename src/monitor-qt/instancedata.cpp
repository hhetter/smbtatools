#include "instancedata.h"

InstanceData::InstanceData(QObject *parent) :
        QObject(parent)
{
    i_stepsize = 5;
    i_max_index = 86400;


    // Configuration data
    configString = "";
    nameString = "";
    fileString = "";
    domainString = "";
    shareString = "";
    userString = "";
    queryParameterString ="";
    portString = "";
    hostString = "";

    idCheck     = false;
    dryrunCheck = false;

    // Process information
    pidString ="";
    socketString="";
    socketConnected = false;

    licenseString = SMBTA_LICENSE;
        

}






#include "instancedata.moc"
