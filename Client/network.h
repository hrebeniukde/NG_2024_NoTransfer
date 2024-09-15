#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QDir>
#include <QDesktopServices>

#include "filestreewidget.h"

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = nullptr);
    ~Network();

    void connectToServer(QHostAddress host, int port);

    bool isAlreadyConnected(QHostAddress host, int port);

    bool isConnectedToServer();

    QString getServerAddress();

    void changeDirectory(QString directoryName);

    void downloadFile(QString filePath, bool isTempFile = true, QString savePath = "");

    void uploadFile(QString filePath, QString savePath);

    void deleteItem(QString itemPath, bool isFolder);

private slots:
    void tcpSocketConnected();

    void tcpSocketDisconnected();

    void tcpSocketReadyRead();

signals:
    void updateFilesList(QVector<ServerFile> filesList, QString directoryPath);

    void downloadFileProgress(int progress);

    void uploadFinished();

    void connectStateChanged(bool connectionState);

private:
    QTcpSocket *tcpSocket;

    QDataStream incomeDataStream;

    QString currentDownloadingFile;
    QString currentDownloadingFileSavePath;

    QString currentUploadingFile;

    void handleIncomingRpc(int rpcId);
};

#endif // NETWORK_H
