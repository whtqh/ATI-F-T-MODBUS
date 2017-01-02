#include "modbusslave.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    modbusSlave w;
    w.show();

    return a.exec();
}
