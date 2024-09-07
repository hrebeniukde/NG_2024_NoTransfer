#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpSocket = new QTcpSocket();
    logger = new Logger(ui->logMessagesEdit);

    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::tcpSocketConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &MainWindow::tcpSocketDisconnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::tcpSocketReadyRead);

    logger->printLog("The application has been started. To get started, you are required to connect to the server.");
}

MainWindow::~MainWindow()
{
    tcpSocket->close();

    delete ui;
    delete tcpSocket;
    delete logger;
}

void MainWindow::on_gitHubButton_clicked()
{
    QDesktopServices::openUrl(QUrl(GITHUB_REPO_URL));
}

void MainWindow::on_aboutButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("About");
    msgBox.setText(
        QString(
            "<center><strong>%1 (%2)</strong></center><br/>"
            "Client application for working with server files with the ability to upload, delete and download them.<br/><br/>"
            "<strong>Social Media:</strong><br/>"
            "- <a href=\"%3\">GitHub</a><br/><br/>"
            "Copyright © 2024 <a href=\"%4\">%5</a>"
        ).arg(APP_NAME, APP_VERSION, GITHUB_REPO_URL, AUTHOR_GITHUB_URL, AUTHOR_NAME)
    );
    msgBox.exec();
}


void MainWindow::on_connectButton_clicked()
{
    QString serverHost = ui->hostLineEdit->text();
    int serverPort = ui->portSpinBox->value();

    if (serverHost.isEmpty() || !serverPort) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You did not specify a server host or port to connect to!");
        msgBox.exec();
        return;
    }

    if (tcpSocket->peerAddress().toString() == serverHost &&
        tcpSocket->peerPort() == serverPort) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You are already connected to this server!");
        msgBox.exec();
        return;
    }

    tcpSocket->connectToHost(QHostAddress(serverHost), serverPort);
}

void MainWindow::tcpSocketConnected()
{
    QString serverHost = tcpSocket->peerAddress().toString();
    QString serverPort = QString::number(tcpSocket->peerPort());

    logger->printLog(QString("Connected to the server (%1:%2).").arg(serverHost, serverPort));
}

void MainWindow::tcpSocketDisconnected()
{
    logger->printLog("Disconnected from the server.");
}

void MainWindow::tcpSocketReadyRead()
{
    // TODO: Implement communication between server and client
}

