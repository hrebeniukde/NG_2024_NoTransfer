#include "mainwindow.h"

#include <QApplication>
#include <QFile>

void loadApplicationStyleSheet(QApplication *application)
{
    QFile styleSheetFile(":/stylesheet.css");

    if (styleSheetFile.open(QFile::ReadOnly)) {
        application->setStyleSheet(styleSheetFile.readAll());
    } else {
        qDebug() << "Failed to load application stylesheet!";
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    loadApplicationStyleSheet(&a);
    w.show();

    return a.exec();
}
