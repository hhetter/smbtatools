#include "showformat.h"
#include "ui_showformat.h"


showformat::showformat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::showformat)
{
    ui->setupUi(this);
}

showformat::~showformat()
{
    delete ui;
}
void showformat::new_Element(int value1, int value2, QSqlQuery q)
{
    while (q.next())
    {
        float graphdata = q.value(value2).toFloat();
        QString bytes = "";
        int size;
        for (size = 0; graphdata > 1024; size++)
        {
                graphdata /= 1024;
        }
       
        switch (size)
        {
        case 1:bytes = " KBytes";break;
        case 2:bytes = " MBytes";break;
        case 3:bytes = " GBytes";break;
        case 4:bytes = " TBytes";break;
        default: bytes = " Bytes";break;
        }

       ui->textBrowser->insertPlainText(q.value(value1).toString());
       ui->textBrowser->insertPlainText(" - ");
       ui->textBrowser->insertPlainText(QString::number(graphdata) + bytes);
       ui->textBrowser->insertPlainText("\n");
    }
}
void showformat::new_Element(int value1, int value2, int value3, QSqlQuery q)
{
    while (q.next())
    {
	float graphdata = q.value(value3).toFloat();
        QString bytes = "";
        int size;
        for (size = 0; graphdata > 1024; size++)
        {
                graphdata /= 1024;
        }

        switch (size)
        {
        case 1:bytes = " KBytes";break;
        case 2:bytes = " MBytes";break;
        case 3:bytes = " GBytes";break;
        case 4:bytes = " TBytes";break;
        default: bytes = " Bytes";break;
        }

       ui->textBrowser->insertPlainText(q.value(value1).toString());
       ui->textBrowser->insertPlainText(" - ");
       ui->textBrowser->insertPlainText(q.value(value2).toString());
       ui->textBrowser->insertPlainText(" - ");
       ui->textBrowser->insertPlainText(QString::number(graphdata) + bytes);
       ui->textBrowser->insertPlainText("\n");
    }
}


#include "showformat.moc"
