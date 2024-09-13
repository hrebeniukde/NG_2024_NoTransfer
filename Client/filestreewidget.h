#ifndef FILESTREEWIDGET_H
#define FILESTREEWIDGET_H

#include <QObject>
#include <QTreeWidget>
#include <QDateTime>
#include <QFileInfo>

struct ServerFile {
    QString name;
    bool isDir;
    uint64_t size;
    QDateTime lastModified;
};

class FilesTreeWidget : public QObject
{
    Q_OBJECT
public:
    explicit FilesTreeWidget(QObject *parent = nullptr);
    ~FilesTreeWidget();

    void setTreeWidget(QTreeWidget *widget);

    void clearTree();

    void addFileToTree(ServerFile file);

private slots:
    void itemDoubleClicked(QTreeWidgetItem *item);

signals:
    void openDirectoryRequest(QString directoryName);

private:
    QTreeWidget *treeWidget;
};

#endif // FILESTREEWIDGET_H
