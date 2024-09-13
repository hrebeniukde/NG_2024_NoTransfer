#include "network.h"

#include "logger.h"
#include "rpc.h"

Network::Network(QObject *parent)
    : QObject{parent}
{
    tcpSocket = new QTcpSocket();

    incomeDataStream.setDevice(tcpSocket);
    incomeDataStream.setVersion(QDataStream::Qt_6_8);

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
    default:
        break;
    }
}
