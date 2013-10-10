#include "smbtaquerydoc.h"

SMBTAqueryDoc::SMBTAqueryDoc()
{
}

SMBTAqueryDoc::SMBTAqueryDoc(QString *query) {

    QString Binary;
    Binary.append("smbtaquery");
    QStringList arguments;
    arguments << "-h" << "seizewell.suse.de";
    QProcess *queryproc = new QProcess();
    queryproc->start(Binary, arguments);
    queryproc->waitForFinished();


}
