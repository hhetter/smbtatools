#include <QtGui/QApplication>
#include "frontend.h"
#include "wmanager.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //frontend w;
    //w.show();
    WManager w;
    w.show();

    return a.exec();
}
