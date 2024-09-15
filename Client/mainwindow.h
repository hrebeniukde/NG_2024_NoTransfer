#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTcpSocket>
#include <QFileDialog>

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

    void on_downloadProgressBar_clicked();

    void on_uploadButton_clicked();

    void on_deleteButton_clicked();

    void updateFilesList(QVector<ServerFile> filesList, QString directoryPath);

    void openDirectoryRequest(QString directoryName);

    void openFileRequest(QString fileName);

    void downloadFileProgress(int progress);

    void uploadFinished();

private:
    Ui::MainWindow *ui;

    FilesTreeWidget *filesTreeWidget;

    Network *network;

    void enableInterfaceInteraction(bool state);
};
#endif // MAINWINDOW_H
