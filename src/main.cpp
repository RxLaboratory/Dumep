#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QTranslator translator;
    translator.load("languages/dumep_" + QLocale::system().name());
    QApplication a(argc, argv);
    a.installTranslator(&translator);
    MainWindow w(&translator,argc,argv);
    w.show();

    return a.exec();
}
