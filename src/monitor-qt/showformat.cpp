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
       ui->textBrowser->insertPlainText(q.value(value1).toString());
       ui->textBrowser->insertPlainText(" - ");
       ui->textBrowser->insertPlainText(q.value(value2).toString());
       ui->textBrowser->insertPlainText("\n");
    }
}
void showformat::new_Element(int value1, int value2, int value3, QSqlQuery q)
{
    while (q.next())
    {
       ui->textBrowser->insertPlainText(q.value(value1).toString());
       ui->textBrowser->insertPlainText(" - ");
       ui->textBrowser->insertPlainText(q.value(value2).toString());
       ui->textBrowser->insertPlainText(" - ");
       ui->textBrowser->insertPlainText(q.value(value3).toString());
       ui->textBrowser->insertPlainText("\n");
    }
}


#include "showformat.moc"
