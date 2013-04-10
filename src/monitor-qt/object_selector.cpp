#include "object_selector.h"
#include "ui_object_selector.h"

object_selector::object_selector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::object_selector)
{




    QString *smbtaquery = "smbtaquery";
    QStringList *arguments;
    arguments << "-H" << "sizewell.suse.de" << "-N" << "smbta" << "-S" << "postgres" << "-M" << "pgsql";
    arguments << "-q" << "'global, list domains;'"
    QObject *parent;
    QProcess *myProcess = new QProcess(parent=0);



    ui->setupUi(this);

    /* Fill the Domainlist at startup */
    myProcess->start(smbtaquery, arguments);


}

object_selector::~object_selector()
{
    delete ui;
}
