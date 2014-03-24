#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "barchartplotter.h"
#include "axisbase.h"


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{

    ui->setupUi(this);

}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWidget::paint()
{

    ui->BarPlot->axisY()->setRanges(max);//!Höhe
    ui->BarPlot->axisY()->setTicks(50, max/20);//!Stichelung und Nummerierung
    ui->BarPlot->axisY()->setPen(QPen(Qt::darkGray));//!Farbe
    ui->BarPlot->axisY()->setMinorTicksPen(QPen(Qt::gray));//!Farbe der Strichelung
    ui->BarPlot->axisY()->setMajorTicksPen(QPen(Qt::darkGray));//!Farbe der Nummerierungsmarkierung
    //ui->BarPlot->axisY()->setMinorGridPen(QPen(Qt::gray));
    ui->BarPlot->axisY()->setMajorGridPen(QPen(Qt::lightGray));//!Farbe des waagrechten Gitters
    ui->BarPlot->axisY()->setTextColor(Qt::black);//!Farbe der Nummern

    ui->BarPlot->axisX()->setPen(QPen(Qt::darkGray));//!Farbe
    ui->BarPlot->axisX()->setMinorTicksPen(QPen(Qt::gray));//!Farbe der Strichelung
    ui->BarPlot->axisX()->setMajorTicksPen(QPen(Qt::darkGray));//!Farbe der Nummermakierung
    ui->BarPlot->axisX()->setMajorGridPen(QPen(Qt::lightGray));//!Farbe des senkrechten gitters
    ui->BarPlot->axisX()->setTextColor(Qt::black);//!Farbe der Nummern

    ui->BarPlot->setBarSize(50, 200);//!Skalierung der Breite
    ui->BarPlot->setBarOpacity(1.0);//!Sichtbarkeit
    //!Hintergrund
    QLinearGradient bg(0,0,0,0);//!Farbschema nutzen
    bg.setCoordinateMode(QGradient::ObjectBoundingMode);
    bg.setColorAt(0, Qt::white);//!Farbschema einstellen
    ui->BarPlot->setBackground(QBrush(bg));//!Farbe ausfüllen
    itemModel = new QStandardItemModel(1,q.size(),this);
    itemModel->setHorizontalHeaderLabels(data); //!Daten aus Liste auf X Achse
    itemModel->setVerticalHeaderLabels(data);//!Daten der einzelnen objekte

    for (int i = 0, x = 1; i < q.size(); i++, x++)
    {
        if (x % 10 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::lightGray, Qt::BackgroundRole);
        }
        else if (x % 9 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::darkGray, Qt::BackgroundRole);
        }
        else if (x % 8 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::magenta, Qt::BackgroundRole);
        }
        else if (x % 7 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::cyan, Qt::BackgroundRole);
        }
        else if (x % 6 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::black, Qt::BackgroundRole);
        }
        else if (x % 5 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::white, Qt::BackgroundRole);
        }
        else if (x % 4 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::yellow, Qt::BackgroundRole);
        }
        else if (x % 3 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::green, Qt::BackgroundRole);
        }
        else if (x % 2 == 0)
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::blue, Qt::BackgroundRole);
        }
        else
        {
            itemModel->setHeaderData(i, Qt::Vertical, Qt::red, Qt::BackgroundRole);
        }
    }

    for (int i = 0; i < q.size(); i++)
    {
        itemModel->setData(itemModel->index(i,i),graphdata[i]);
    }

    ui->BarPlot->setModel(itemModel);

    ui->BarPlot->setBarType(QSint::BarChartPlotter::Columns);
    ui->BarPlot->repaint();
}

void MainWidget::new_Element(int value1, int value2, QSqlQuery q)
{
   int maxq = 0;
   this->graphdata = new int[q.size()];
   for(int i = 0; i < q.size() && q.next(); i++)
   {
        this->graphdata[i] = q.value(value2).toInt();
        this->data.append(q.value(value1).toString());
       if(maxq < this->graphdata[i])
        {
            maxq = this->graphdata[i];
        }
   }
   max = maxq+10;
   this->q = q;
   paint();
}

void MainWidget::new_Element(int value1, int value2, int value3, QSqlQuery q)
{
    int maxq = 0;
    QString showdata;
    this->graphdata = new int[q.size()];
    for(int i = 0; i < q.size() && q.next(); i++)
    {
         this->graphdata[i] = q.value(value2).toInt();
        if (value3 == -1)
        {
            this->graphdata[i] /= (1024*1024);
            showdata = q.value(value1).toString();
        }
        else
        {
            showdata = q.value(value1).toString() + " - " + q.value(value3).toString();
        }
         this->data.append(showdata);
         if(maxq < this->graphdata[i])
         {
             maxq = this->graphdata[i];
         }
    }
    max = maxq+10;
    this->q = q;
    paint();
}

#include "mainwidget.moc"
