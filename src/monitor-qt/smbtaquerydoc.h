#ifndef SMBTAQUERYDOC_H
#define SMBTAQUERYDOC_H

#include <QDomDocument>
#include <QProcess>

class SMBTAqueryDoc : public QDomDocument
{
public:
    SMBTAqueryDoc();
    SMBTAqueryDoc( QString *query );
};

#endif // SMBTAQUERYDOC_H
