#include "configform.h"
#include "ui_configform.h"

ConfigForm::ConfigForm(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ConfigForm)
{
        ui->setupUi(this);

        ui->queryTypeButton->addItem("User");
        ui->queryTypeButton->addItem("File");
        ui->queryTypeButton->addItem("Share");
        ui->queryTypeButton->addItem("Domain");
        ui->queryTypeButton->addItem("Global");
        ui->queryTypeButton->setCurrentIndex(0);

        configString = new QString;

        i_monitortime = 600;
        i_intervaltime = 600;

        qDebug()<<"Class constructor ConfigForm";
        //    cf_readconfig();
}

ConfigForm::~ConfigForm()
{
        delete ui;
}


void ConfigForm::cf_readconfig(){

        *configString = "";

        qDebug()<<"Current index: " <<ui->queryTypeButton->currentIndex();
        int i_switch = ui->queryTypeButton->currentIndex();
        qDebug()<<"i_switch: " <<i_switch;

        switch(ui->queryTypeButton->currentIndex())
        {
        case 0:
                configString->append(" -u "+ui->queryParameterLine->text());
                qDebug()<< "Configstring: " << *configString;
                break;
        case 1:
                configString->append(" -f "+ui->queryParameterLine->text());
                qDebug()<< "Configstring: " << *configString;
                break;
        case 2:
                configString->append(" -s "+ui->queryParameterLine->text());
                qDebug()<< "Configstring: " << *configString;
                break;
        case 3:
                configString->append(" -d "+ui->queryParameterLine->text());
                qDebug()<< "Configstring: " << *configString;
                break;
        case 4:
                configString->append(" -g "+ui->queryParameterLine->text());
                qDebug()<< "Configstring: " << *configString;
                break;
        default:
                qDebug()<<"nil";
        }

        if(ui->idCheckbox->isChecked()){
                configString->append(" -I 1");
        }else(configString->append(" -I 0"));
        if(ui->dryrunCheckbox->isChecked()){
                *configString = " -x";
        }









        ////
        // Parse the input of the configuration form and put it together to a QString. The QString will be handled over to the monitoring process.
        // password is not implemented in the smbtamonitor-gen so it is disabled here. Should I leave it here or completely disable it?
        // queryshares not implemented yet
        // Save/Load not implemented yet, and also not part of this method
        // **wip**


        //    qDebug()<<"cf_readconfig";

        ////
        // Reset configString in case it already contains values
        /*Taken out, here sits the queryTypeButton now
    if(ui->hostLine->text() != ""){
        configString->append(" -h "+ui->hostLine->text());
    }
    *//*
    if(ui->portLine->text() != ""){
        configString->append(" -i "+ui->portLine->text());
    }
    if(ui->userLine->text() != ""){
        configString->append(" -u "+ui->userLine->text());
    }
    if(ui->fileLine->text() != ""){
        configString->append(" -f "+ui->userLine->text());
    }
    if(ui->domainLine->text() != ""){
        configString->append(" -d "+ui->userLine->text());
    }
    if(ui->shareLine->text() != ""){
        configString->append(" -s "+ui->userLine->text());
    }
    */

        //    if(ui->passwordLine->text() != ""){
        //        configString->append(" - "+ui->passwordLine->text());
        //    }

        /*
      No use atm
    if(ui->monitorLine->text() != ""){
        i_monitortime = ui->monitorLine->text().toInt();
    }
    if(ui->intervalLine->text() != ""){
        i_intervaltime = 600 * ui->intervalLine->text().toInt();
    }
    */



        //    qDebug()<<"*configString: "<<*configString;
        //    qDebug()<<"i_monitortime: "<<i_monitortime;
        //    qDebug()<<"i_intervaltime: "<< i_intervaltime;



}


#include "configform.moc"
