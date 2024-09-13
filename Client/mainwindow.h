#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTcpSocket>

#include "network.h"
#include "filestreewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_gitHubButton_clicked();

    void on_aboutButton_clicked();

    void on_connectButton_clicked();

    void updateFilesList(QVector<ServerFile> filesList, QString directoryPath);

    void openDirectoryRequest(QString directoryName);

private:
    Ui::MainWindow *ui;

    FilesTreeWidget *filesTreeWidget;

    Network *network;

    QString currentDirectoryPath;
};
#endif // MAINWINDOW_H
