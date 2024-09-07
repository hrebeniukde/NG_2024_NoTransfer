#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTcpSocket>

#include "logger.h"

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

    void tcpSocketConnected();

    void tcpSocketDisconnected();

    void tcpSocketReadyRead();

private:
    Ui::MainWindow *ui;

    QTcpSocket *tcpSocket;

    Logger *logger;
};
#endif // MAINWINDOW_H
