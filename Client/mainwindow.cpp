#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    network = new Network();

    Logger::initLogger(ui->logMessagesEdit);

    Logger::printLog("The application has been started. To get started, you are required to connect to the server.");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete network;
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
    QHostAddress serverHost = QHostAddress(ui->hostLineEdit->text());
    int serverPort = ui->portSpinBox->value();

    if (serverHost.isNull() || !serverPort) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You did not specify a server host or port to connect to!");
        msgBox.exec();
        return;
    }

    if (network->isAlreadyConnected(serverHost, serverPort)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("You are already connected to this server!");
        msgBox.exec();
        return;
    }

    network->connectToServer(serverHost, serverPort);
}

