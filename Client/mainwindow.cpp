#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Logger::initLogger(ui->logMessagesEdit);

    filesTreeWidget = new FilesTreeWidget();
    filesTreeWidget->setTreeWidget(ui->filesTreeWidget);

    network = new Network();

    connect(network, &Network::updateFilesList, this, &MainWindow::updateFilesList);
    connect(network, &Network::downloadFileProgress, this, &MainWindow::downloadFileProgress);
    connect(filesTreeWidget, &FilesTreeWidget::openDirectoryRequest, this, &MainWindow::openDirectoryRequest);
    connect(filesTreeWidget, &FilesTreeWidget::openFileRequest, this, &MainWindow::openFileRequest);

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

void MainWindow::on_downloadProgressBar_clicked()
{
    QString fileName = filesTreeWidget->getSelectedFile();
    if (fileName.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The file is not selected or you have selected a folder. Try again.");
        msgBox.exec();
        return;
    }

    QString saveDirectory = QFileDialog::getExistingDirectory(this, "Select folder to save file", "", QFileDialog::ShowDirsOnly);
    if (saveDirectory.isEmpty())
        return;

    network->downloadFile(currentDirectoryPath + "/" + fileName, false, saveDirectory);
}

void MainWindow::updateFilesList(QVector<ServerFile> filesList, QString directoryPath)
{
    currentDirectoryPath = directoryPath;

    filesTreeWidget->clearTree();

    for (int index = 0; index < filesList.size(); index++) {
        filesTreeWidget->addFileToTree(filesList.at(index));
    }
}

void MainWindow::openDirectoryRequest(QString directoryName)
{
    QString newDirectoryPath;

    if (directoryName == "..") {
        newDirectoryPath = currentDirectoryPath.left(currentDirectoryPath.lastIndexOf('/'));
    } else {
        newDirectoryPath = currentDirectoryPath + "/" + directoryName;
    }

    network->changeDirectory(newDirectoryPath);
}

void MainWindow::openFileRequest(QString fileName)
{
    network->downloadFile(currentDirectoryPath + "/" + fileName);
}

void MainWindow::downloadFileProgress(int progress)
{
    if (progress >= 99 || progress < 0) {
        enableInterfaceInteraction(true);

        ui->downloadProgressBar->setValue(100);
        ui->downloadProgressBar->setFormat("Download file");
        return;
    }

    enableInterfaceInteraction(false);

    ui->downloadProgressBar->setValue(progress);
    ui->downloadProgressBar->setFormat("%p%");
}

void MainWindow::enableInterfaceInteraction(bool state)
{
    ui->connectButton->setEnabled(state);

    ui->downloadProgressBar->setEnabled(state);
    ui->uploadButton->setEnabled(state);
    ui->deleteButton->setEnabled(state);

    ui->filesTreeWidget->setEnabled(state);
}

