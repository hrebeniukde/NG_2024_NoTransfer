#include "mainwindow.h"

#include <QApplication>
#include <QFile>

void loadApplicationStyleSheet(QApplication *application)
{
    QFile styleSheetFile(":/assets/stylesheet.css");

    if (styleSheetFile.open(QFile::ReadOnly)) {
        QString style = styleSheetFile.readAll();

        // SpinBox Fix for Windows 11
        QString osVersion = QSysInfo::prettyProductName();
        if (osVersion.contains("Windows 11")) {
            style.append("#portSpinBox { padding: 7px 0px 7px 0px; }");
        } else {
            style.append("#portSpinBox { padding: 7px 7px; }");
        }

        application->setStyleSheet(style);
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
