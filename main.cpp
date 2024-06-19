#include <QApplication>
#include "turingwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TuringWindow MT;

    MT.show();

    return a.exec();
}
