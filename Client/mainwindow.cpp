#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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

