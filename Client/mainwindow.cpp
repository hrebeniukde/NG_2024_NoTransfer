#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "logger.h"
#include "util.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Logger::initLogger(ui->logMessagesEdit);

    filesTreeWidget = new FilesTreeWidget();
    filesTreeWidget->setTreeWidget(ui->filesTreeWidget);

    network = new Network();

    connect(network, &Network::connectStateChanged, this, &MainWindow::connectStateChanged);
    connect(network, &Network::updateFilesList, this, &MainWindow::updateFilesList);
    connect(network, &Network::downloadFileProgress, this, &MainWindow::downloadFileProgress);
    connect(network, &Network::uploadFinished, this, &MainWindow::uploadFinished);
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
            "Client application for working with server files with the ability to download, upload, create, rename and delete them.<br/><br/>"
            "<strong>Social Media:</strong><br/>"
            "- <a href=\"%3\" style=\"color: white;\">GitHub</a><br/><br/>"
            "Copyright © 2024 <a href=\"%4\" style=\"color: white;\">%5</a>"
            ).arg(APP_NAME, APP_VERSION, GITHUB_REPO_URL, AUTHOR_GITHUB_URL, AUTHOR_NAME)
        );
    msgBox.exec();
}


void MainWindow::on_connectButton_clicked()
{
    QHostAddress serverHost = QHostAddress(ui->hostLineEdit->text());
    int serverPort = ui->portSpinBox->value();

    if (serverHost.isNull() || !serverPort) {
        Util::sendWarningAlert("You did not specify a server host or port to connect to!");
        return;
    }

    if (network->isAlreadyConnected(serverHost, serverPort)) {
        Util::sendWarningAlert("You are already connected to this server!");
        return;
    }

    network->connectToServer(serverHost, serverPort);
}

void MainWindow::on_downloadProgressBar_clicked()
{
    if (!Util::isConnectedToServer(network))
        return;

    QPair<QString, bool> item = filesTreeWidget->getSelectedItem();
    if (item.first.isEmpty()) {
        Util::sendWarningAlert("You have not selected the file you want to download.");
        return;
    }

    if (item.second) {
        Util::sendWarningAlert("Unable to download entire folder, select file.");
        return;
    }

    QString saveDirectory = QFileDialog::getExistingDirectory(this, "Select folder to save file", "", QFileDialog::ShowDirsOnly);
    if (saveDirectory.isEmpty())
        return;

    network->downloadFile(Util::getItemFullPath(item.first), false, saveDirectory);
}

void MainWindow::on_uploadButton_clicked()
{
    if (!Util::isConnectedToServer(network))
        return;

    QString filePath = QFileDialog::getOpenFileName(this, "Select file to upload", "", "All Files (*)");
    if (filePath.isEmpty())
        return;

    enableInterfaceInteraction(false);

    network->uploadFile(filePath);
}

void MainWindow::on_createButton_clicked()
{
    if (!Util::isConnectedToServer(network))
        return;

    bool dialogResult;
    QString itemName = QInputDialog::getText(this, APP_NAME, "Enter the desired file or folder name:", QLineEdit::Normal, "", &dialogResult);
    if (!dialogResult || itemName.isEmpty())
        return;

    network->createItem(itemName);
}

void MainWindow::on_renameButton_clicked()
{
    if (!Util::isConnectedToServer(network))
        return;

    QPair<QString, bool> item = filesTreeWidget->getSelectedItem();
    if (item.first.isEmpty()) {
        Util::sendWarningAlert("You have not selected a folder or file to rename.");
        return;
    }

    bool dialogResult;
    QString newItemName = QInputDialog::getText(this, APP_NAME, "Enter the desired new name:", QLineEdit::Normal, "", &dialogResult);
    if (!dialogResult || newItemName.isEmpty())
        return;

    network->renameItem(Util::getItemFullPath(item.first), newItemName);
}

void MainWindow::on_deleteButton_clicked()
{
    if (!Util::isConnectedToServer(network))
        return;

    QPair<QString, bool> item = filesTreeWidget->getSelectedItem();
    if (item.first.isEmpty()) {
        Util::sendWarningAlert("You have not selected a folder or file to delete.");
        return;
    }

    network->deleteItem(Util::getItemFullPath(item.first), item.second);
}

void MainWindow::updateFilesList(QVector<ServerFile> filesList, QString directoryPath)
{
    Util::currentDirectoryPath = directoryPath;

    filesTreeWidget->clearTree();

    for (int index = 0; index < filesList.size(); index++) {
        filesTreeWidget->addFileToTree(filesList.at(index));
    }
}

void MainWindow::openDirectoryRequest(QString directoryName)
{
    QString newDirectoryPath;

    if (directoryName == "..") {
        newDirectoryPath = Util::getItemParentDirectory(Util::currentDirectoryPath);
    } else {
        newDirectoryPath = Util::getItemFullPath(directoryName);
    }

    network->changeDirectory(newDirectoryPath);
}

void MainWindow::openFileRequest(QString fileName)
{
    network->downloadFile(Util::getItemFullPath(fileName));
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

void MainWindow::uploadFinished()
{
    enableInterfaceInteraction(true);
    network->changeDirectory(Util::currentDirectoryPath);
}

void MainWindow::connectStateChanged(bool connectionState)
{
    QString windowTitle;

    if (connectionState) {
        windowTitle = QString("%1 - %2").arg(network->getServerAddress(), APP_NAME);
    } else {
        windowTitle = QString("No connection - %1").arg(APP_NAME);
    }

    setWindowTitle(windowTitle);
}

void MainWindow::enableInterfaceInteraction(bool state)
{
    ui->connectButton->setEnabled(state);

    ui->downloadProgressBar->setEnabled(state);
    ui->uploadButton->setEnabled(state);

    ui->createButton->setEnabled(state);
    ui->renameButton->setEnabled(state);
    ui->deleteButton->setEnabled(state);

    ui->filesTreeWidget->setEnabled(state);
}
