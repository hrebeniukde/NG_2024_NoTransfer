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

    void changeDirectory(QString directoryName);

    void downloadFile(QString filePath, bool tempFile = true, QString savePath = "");

private slots:
    void tcpSocketConnected();

    void tcpSocketDisconnected();

    void tcpSocketReadyRead();

signals:
    void updateFilesList(QVector<ServerFile> filesList, QString directoryPath);

    void downloadFileProgress(int progress);

private:
    QTcpSocket *tcpSocket;

    QDataStream incomeDataStream;

    QString currentDownloadingFile;
    QString currentDownloadingFileSavePath;

    void handleIncomingRpc(int rpcId);
};

#endif // NETWORK_H
