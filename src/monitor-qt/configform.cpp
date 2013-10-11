#include "configform.h"
#include "ui_configform.h"

ConfigForm::ConfigForm(InstanceData *idata, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ConfigForm)
{

        ldata = idata;

        ui->setupUi(this);

        ui->loadButton->hide();
        ui->saveButton->hide();
        //ui->dryrunCheckbox->hide(); //we want it visible
        ui->dryrunCheckbox->setChecked(true);

        ui->queryTypeButton->addItem("User");
        ui->queryTypeButton->addItem("File");
        ui->queryTypeButton->addItem("Share");
        ui->queryTypeButton->addItem("Domain");
        ui->queryTypeButton->addItem("Global");
        ui->queryTypeButton->setCurrentIndex(0);

        ui->licenseLabel->setText(ldata->licenseString);

        configString = new QString;
        hostString   = new QString;
        fileString   = new QString;
        portString   = new QString;
        shareString  = new QString;
        userString   = new QString;
        domainString = new QString;


        i_monitortime = 600;
        i_intervaltime = 600;

        connect(ui->queryTypeButton, SIGNAL(currentIndexChanged(int)),this,
                SLOT(cf_hidefield()));
        connect(ui->dryrunCheckbox, SIGNAL(stateChanged(int)),this,
                SLOT(cf_hideall()));


}

ConfigForm::~ConfigForm()
{
        delete ui;
}

void ConfigForm::cf_hideall()
{
        if(ui->dryrunCheckbox->isChecked())
        {
                ui->queryParameterLine->setText("");
                ui->queryParameterLine->setDisabled(1);
                ui->hostLine->setText("");
                ui->hostLine->setDisabled(1);
                ui->portLine->setText("");
                ui->portLine->setDisabled(1);
                ui->queryTypeButton->setDisabled(1);



        }else
        {
                ui->queryParameterLine->setDisabled(0);
                ui->portLine->setDisabled(0);
                ui->hostLine->setDisabled(0);
                ui->queryTypeButton->setDisabled(0);
        }

}


void ConfigForm::cf_hidefield()
{
        if(ui->queryTypeButton->currentIndex() == 4)
        {
                ui->queryParameterLine->setText("");
                ui->queryParameterLine->setDisabled(1);
        }else
        {
                ui->queryParameterLine->setDisabled(0);
        }

}

/*
 * No input validation is done - needs to be implemented too before the release.
 *
 */
void ConfigForm::cf_readconfig(){

        *configString = "";
        *hostString   = "";
        *fileString   = "";
        *portString   = "";
        *shareString  = "";
        *userString   = "";
        *domainString = "";


        switch(ui->queryTypeButton->currentIndex())
        {
        case 0:
                configString->append(" -u "+ui->queryParameterLine->text());
                userString->append(ui->queryParameterLine->text());
                //
                ldata->userString = ui->queryParameterLine->text();
                ldata->configString.append(" -u "+ui->queryParameterLine->text());
                break;
        case 1:
                configString->append(" -f "+ui->queryParameterLine->text());
                fileString->append(ui->queryParameterLine->text());
                //
                ldata->fileString = ui->queryParameterLine->text();
                ldata->configString.append(" -f "+ui->queryParameterLine->text());
                break;
        case 2:
                configString->append(" -s "+ui->queryParameterLine->text());
                shareString->append(ui->queryParameterLine->text());
                //
                ldata->shareString = ui->queryParameterLine->text();
                ldata->configString.append(" -s "+ui->queryParameterLine->text());
                break;
        case 3:
                configString->append(" -d "+ui->queryParameterLine->text());
                domainString->append(ui->queryParameterLine->text());
                //
                ldata->domainString = ui->queryParameterLine->text();
                ldata->configString.append(" -d "+ui->queryParameterLine->text());
                break;
        case 4:
                configString->append(" -g ");
                //
                ldata->configString = ui->queryParameterLine->text();
                ldata->configString.append(" -g ");
                break;
        default:
                qDebug()<<"nil";
        }

        if(ui->hostLine->text() != ""){
                configString->append(" -h "+ui->hostLine->text());
                hostString->append(ui->hostLine->text());
                //
                ldata->hostString = ui->hostLine->text();
                ldata->configString.append(" -h "+ui->hostLine->text());
        }

        if(ui->portLine->text() != ""){
                configString->append(" -i "+ui->portLine->text());
                portString->append(ui->portLine->text());
                //
                ldata->portString = ui->portLine->text();
                ldata->configString.append(" -i "+ui->portLine->text());
        }


        if(ui->idCheckbox->isChecked()){
                configString->append(" -I 1");
                //
                ldata->idCheck = true;
                ldata->configString.append(" -I 1");
        }else(configString->append(" -I 0"));

        if(ui->dryrunCheckbox->isChecked()){
                *configString = " -x";
                //
                ldata->dryrunCheck = true;
                ldata->configString = " -x";
        }
        //qDebug()<< "Configstring: " << *configString;




}


#include "configform.moc"
