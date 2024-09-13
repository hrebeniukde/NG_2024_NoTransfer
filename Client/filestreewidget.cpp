#include "filestreewidget.h"

FilesTreeWidget::FilesTreeWidget(QObject *parent)
    : QObject{parent}
{}

FilesTreeWidget::~FilesTreeWidget()
{
    delete treeWidget;
}

void FilesTreeWidget::setTreeWidget(QTreeWidget *widget)
{
    treeWidget = widget;

    treeWidget->setHeaderLabels(QStringList() << "Name" << "Size" << "Type" << "Date Modified");

    connect (treeWidget, &QTreeWidget::itemDoubleClicked, this, &FilesTreeWidget::itemDoubleClicked);
}

void FilesTreeWidget::clearTree()
{
    treeWidget->clear();

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeWidget);
    treeItem->setIcon(0, QIcon(":/assets/folder_icon.png"));
    treeItem->setText(0, "..");
}

void FilesTreeWidget::addFileToTree(ServerFile file)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeWidget);
    QLocale locale = QLocale::system();
    QFileInfo fileInfo(file.name);

    treeItem->setIcon(0, QIcon(file.isDir ? ":/assets/folder_icon.png" : ":/assets/file_icon.png"));
    treeItem->setText(0, file.name);
    treeItem->setText(1, locale.formattedDataSize(file.size));
    treeItem->setText(2, file.isDir ? "Folder" : "File ." + fileInfo.suffix());
    treeItem->setText(3, file.lastModified.toString("dd.MM.yyyy hh:ss:mm"));

    treeWidget->resizeColumnToContents(0);
}

void FilesTreeWidget::itemDoubleClicked(QTreeWidgetItem *item)
{
    QString fileName = item->text(0);
    QString fileType = item->text(2);

    if (fileType.contains("File"))
        return;

    emit openDirectoryRequest(fileName);
}
