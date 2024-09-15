#include "server.h"

#include "rpc.h"
#include "util.h"

Server::Server(QObject *parent)
    : QObject{parent}
{
    tcpServer = new QTcpServer();

    connect (tcpServer, &QTcpServer::newConnection, this, &Server::clientConnected);
}

Server::~Server()
{
    for (int clientIndex = 0; clientIndex < clients.size(); clientIndex++) {
        auto &client = clients[clientIndex];

        DEBUG << "Closing the connection to " << getClientFormattedAddress(client.tcpSocket);
        client.tcpSocket->close();

        delete client.tcpSocket;
        delete client.incomeDataStream;

        clients.removeAt(clientIndex);
    }

    if (tcpServer->isListening()) {
        DEBUG << "Closing server";
        tcpServer->close();
    }

    delete tcpServer;
}

void Server::start(QString host, int port)
{
    if (tcpServer->listen(QHostAddress(host), port)) {
        DEBUG << "The server has been started, its address is " << host << ":" << port;
    } else {
        DEBUG << "There was an error when starting the server: " << tcpServer->errorString();
    }
}

void Server::clientConnected()
{
    QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
    DEBUG << "New connection: " << getClientFormattedAddress(tcpSocket);

    connect (tcpSocket, &QTcpSocket::disconnected, this, &Server::clientDisconnected);
    connect (tcpSocket, &QTcpSocket::readyRead, this, &Server::incomingDataFromClient);

    ClientData client;
    client.tcpSocket = tcpSocket;
    client.incomeDataStream = new QDataStream();
    client.incomeDataStream->setDevice(tcpSocket);
    client.incomeDataStream->setVersion(QDataStream::Qt_6_7);
    clients.append(client);

    sendFilesList(client);
}

void Server::clientDisconnected()
{
    QTcpSocket *tcpSocket = (QTcpSocket *)sender();

    for (int clientIndex = 0; clientIndex < clients.size(); clientIndex++) {
        auto &client = clients[clientIndex];
        if (client.tcpSocket != tcpSocket)
            continue;

        DEBUG << "Client " << getClientFormattedAddress(client.tcpSocket) << " has been disconnected";

        client.tcpSocket->close();
        delete client.tcpSocket;
        delete client.incomeDataStream;

        clients.removeAt(clientIndex);
        break;
    }
}

void Server::incomingDataFromClient()
{
    QTcpSocket *tcpSocket = (QTcpSocket *)sender();

    for (auto client : clients) {
        if (client.tcpSocket != tcpSocket)
            continue;

        client.incomeDataStream->startTransaction();

        int rpcId;
        *client.incomeDataStream >> rpcId;
        handleIncomingRpc(client, rpcId);
        break;
    }
}

void Server::handleIncomingRpc(ClientData client, int rpcId)
{
    switch (rpcId) {
    case RPC_CHANGE_DIRECTORY: {
        QString newDirectoryPath;

        *client.incomeDataStream >> newDirectoryPath;

        if (!client.incomeDataStream->commitTransaction())
            return;

        sendFilesList(client, newDirectoryPath);
        break;
    }
    case RPC_DOWNLOAD_FILE: {
        QString filePath;
        bool isTempFile;

        *client.incomeDataStream >> filePath;
        *client.incomeDataStream >> isTempFile;

        if (!client.incomeDataStream->commitTransaction())
            return;

        DEBUG << "New download request from " << getClientFormattedAddress(client.tcpSocket);
        DEBUG << "Requested file: \"" << filePath << "\"";

        sendFile(client, filePath, isTempFile);
        break;
    }
    case RPC_UPLOAD_FILE: {
        QString filePath;
        QByteArray fileContent;

        *client.incomeDataStream >> filePath;
        *client.incomeDataStream >> fileContent;

        if (!client.incomeDataStream->commitTransaction())
            return;

        QFile file(filePath);
        file.open(QIODevice::WriteOnly);
        file.write(fileContent);
        file.close();

        sendUploadFinished(client);
        break;
    }
    case RPC_DELETE_FILE: {
        QString itemPath;
        bool isFolder;

        *client.incomeDataStream >> itemPath;
        *client.incomeDataStream >> isFolder;

        if (!client.incomeDataStream->commitTransaction())
            return;

        if (isFolder) {
            QDir directory(itemPath);
            if (!directory.exists())
                return;

            directory.removeRecursively();
        } else {
            QFile file(itemPath);
            if (!file.exists())
                return;

            file.remove();
        }

        sendFilesList(client, Util::getItemParentDirectory(itemPath));
        break;
    }
    default:
        break;
    }
}

void Server::sendFilesList(ClientData client, QString directoryPath)
{
    if (directoryPath.isEmpty())
        directoryPath = SHARED_FOLDER_PATH;

    QDir directory(directoryPath);
    QFileInfoList entryInfoList = directory.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::DirsFirst);

    QByteArray sendDataBlock;
    QDataStream sendDataStream(&sendDataBlock, QIODeviceBase::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_6_8);

    sendDataStream << (int)RPC_SEND_FILES_LIST;
    sendDataStream << directoryPath;

    for (const auto& entry : entryInfoList) {
        sendDataStream << entry.fileName();
        sendDataStream << entry.isDir();
        sendDataStream << (uint64_t) entry.size();
        sendDataStream << entry.lastModified();
    }

    client.tcpSocket->write(sendDataBlock);
}

void Server::sendFile(ClientData client, QString filePath, bool isTempFile)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray sendDataBlock;
    QDataStream sendDataStream(&sendDataBlock, QIODeviceBase::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_6_8);

    sendDataStream << (int)RPC_SEND_FILE;
    sendDataStream << isTempFile;

    QByteArray fileContent = file.readAll();
    sendDataStream << fileContent.size();
    sendDataStream << fileContent;

    file.close();

    client.tcpSocket->write(sendDataBlock);

    DEBUG << "File \"" << filePath << "\" has been sent to " << getClientFormattedAddress(client.tcpSocket);
}

void Server::sendUploadFinished(ClientData client)
{
    QByteArray sendDataBlock;
    QDataStream sendDataStream(&sendDataBlock, QIODeviceBase::WriteOnly);
    sendDataStream.setVersion(QDataStream::Qt_6_8);

    sendDataStream << (int)RPC_UPLOAD_FILE_FINISHED;

    client.tcpSocket->write(sendDataBlock);
}

QString Server::getClientFormattedAddress(QTcpSocket *client)
{
    return client->peerAddress().toString() + ":" + QString::number(client->peerPort());
}
