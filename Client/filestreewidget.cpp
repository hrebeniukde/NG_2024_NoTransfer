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

    treeWidget->setHeaderLabels(QStringList() << "Name" << "Date Modified" << "Type" << "Size");

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
    QLocale locale(QLocale::English);
    QFileInfo fileInfo(file.name);

    treeItem->setIcon(0, QIcon(file.isDir ? ":/assets/folder_icon.png" : ":/assets/file_icon.png"));
    treeItem->setText(0, file.name);
    treeItem->setText(1, file.lastModified.toString("dd.MM.yyyy hh:mm:ss"));
    treeItem->setText(2, file.isDir ? "Folder" : "File ." + fileInfo.suffix());
    treeItem->setText(3, file.isDir ? "" : locale.formattedDataSize(file.size));

    treeWidget->resizeColumnToContents(0);
    treeWidget->resizeColumnToContents(1);
}

void FilesTreeWidget::itemDoubleClicked(QTreeWidgetItem *item)
{
    QString fileName = item->text(0);
    QString fileType = item->text(2);

    if (fileType.contains("File")) {
        emit openFileRequest(fileName);
    } else {
        emit openDirectoryRequest(fileName);
    }
}

QString FilesTreeWidget::getSelectedFile()
{
    QList<QTreeWidgetItem *> itemList;
    itemList = treeWidget->selectedItems();

    if (itemList.isEmpty())
        return "";

    QString itemType = itemList.at(0)->text(2);
    if (itemType == "Folder" || itemType == "")
        return "";

    return itemList.at(0)->text(0);
}
