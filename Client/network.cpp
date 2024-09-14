#include "network.h"

#include "config.h"
#include "logger.h"
#include "rpc.h"

Network::Network(QObject *parent)
    : QObject{parent}
{
    tcpSocket = new QTcpSocket();

    incomeDataStream.setDevice(tcpSocket);
    incomeDataStream.setVersion(QDataStream::Qt_6_8);

    currentDownloadingFile.clear();
    currentDownloadingFileSavePath.clear();

    connect(tcpSocket, &QTcpSocket::connected, this, &Network::tcpSocketConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Network::tcpSocketDisconnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Network::tcpSocketReadyRead);
}

Network::~Network()
{
    tcpSocket->close();

    delete tcpSocket;
}

void Network::connectToServer(QHostAddress host, int port)
{
    tcpSocket->connectToHost(QHostAddress(host), port);
}

bool Network::isAlreadyConnected(QHostAddress host, int port)
{
    return tcpSocket->peerAddress() == host && tcpSocket->peerPort() == port;
}

void Network::changeDirectory(QString newDirectoryPath)
{
    QByteArray sendDataBlock;
    QDataStream sendDataStream(&sendDataBlock, QIODeviceBase::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_6_8);

    sendDataStream << (int)RPC_CHANGE_DIRECTORY;
    sendDataStream << newDirectoryPath;

    tcpSocket->write(sendDataBlock);
}

void Network::downloadFile(QString filePath, bool tempFile, QString savePath)
{
    currentDownloadingFile = filePath.mid(filePath.lastIndexOf('/') + 1);

    if (!tempFile && !savePath.isEmpty())
        currentDownloadingFileSavePath = savePath;

    QByteArray sendDataBlock;
    QDataStream sendDataStream(&sendDataBlock, QIODeviceBase::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_6_8);

    sendDataStream << (int)RPC_DOWNLOAD_FILE;
    sendDataStream << filePath;
    sendDataStream << tempFile;

    tcpSocket->write(sendDataBlock);

    Logger::printLog(QString("File \"%1\" download has started.").arg(currentDownloadingFile));
    Logger::printLog("<b><i>WARNING!</i></b> The application interface is temporarily disabled while downloading files.");
}

void Network::tcpSocketConnected()
{
    QString serverHost = tcpSocket->peerAddress().toString();
    QString serverPort = QString::number(tcpSocket->peerPort());

    Logger::printLog(QString("Connected to the server (%1:%2).").arg(serverHost, serverPort));
}

void Network::tcpSocketDisconnected()
{
    Logger::printLog("Disconnected from the server.");
}

void Network::tcpSocketReadyRead()
{
    incomeDataStream.startTransaction();

    int rpcId;
    incomeDataStream >> rpcId;

    handleIncomingRpc(rpcId);
}

void Network::handleIncomingRpc(int rpcId)
{
    switch (rpcId) {
    case RPC_SEND_FILES_LIST: {
        QVector<ServerFile> filesList;
        QString directoryPath;

        incomeDataStream >> directoryPath;

        while (incomeDataStream.device()->bytesAvailable() > 0) {
            ServerFile file;

            incomeDataStream >> file.name;
            incomeDataStream >> file.isDir;
            incomeDataStream >> file.size;
            incomeDataStream >> file.lastModified;

            filesList.push_back(file);
        }

        if (!incomeDataStream.commitTransaction())
            return;

        emit updateFilesList(filesList, directoryPath);
        break;
    }
    case RPC_SEND_FILE: {
        bool tempFile;
        qsizetype fileSize;
        QByteArray fileContent;

        incomeDataStream >> tempFile;
        incomeDataStream >> fileSize;

        qint64 downloadedPartSize = tcpSocket->size() - tcpSocket->pos();
        int downloadProgress = (double(downloadedPartSize) / fileSize) * 100;
        emit downloadFileProgress(downloadProgress);

        incomeDataStream >> fileContent;

        if (!incomeDataStream.commitTransaction())
            return;

        QString fileSavePath;
        if (tempFile) {
            QString tempAppFolder = QString("%1/%2/").arg(QDir::tempPath(), APP_NAME);
            fileSavePath = tempAppFolder + currentDownloadingFile;
            QDir().mkpath(tempAppFolder);
        } else {
            fileSavePath = currentDownloadingFileSavePath + "/" + currentDownloadingFile;
        }

        QFile file(fileSavePath);
        if (file.exists())
            file.remove();

        file.open(QIODevice::WriteOnly);
        file.write(fileContent);
        file.close();

        Logger::printLog(QString("File \"%1\" download has finished. File has downloaded to \"%2\".").arg(currentDownloadingFile, fileSavePath));

        if (tempFile)
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileSavePath));
    }
    default:
        break;
    }
}
