#include "navire.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NAVIRE w;
    w.show();
    return a.exec();
}
