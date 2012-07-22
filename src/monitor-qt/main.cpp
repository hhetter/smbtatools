#include <QtGui/QApplication>
#include "frontend.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frontend w;
    w.show();

    return a.exec();
}
