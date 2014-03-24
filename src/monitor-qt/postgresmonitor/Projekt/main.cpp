#include "pgformular.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pgformular w;
    w.show();

    return a.exec();
}
