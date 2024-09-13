#include "server.h"
#include "rpc.h"

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

        DEBUG << "Closing the connection to " << client.tcpSocket->peerAddress().toString() << "...";
        client.tcpSocket->close();

        delete client.tcpSocket;
        delete client.incomeDataStream;

        clients.removeAt(clientIndex);
    }

    if (tcpServer->isListening()) {
        DEBUG << "Closing server...";
        tcpServer->close();
    }

    delete tcpServer;
}

void Server::start(QString host, int port)
{
    if (tcpServer->listen(QHostAddress(host), port)) {
        DEBUG << "The server has been started, its address is " << host << ":" << port << "...";
    } else {
        DEBUG << "There was an error when starting the server: " << tcpServer->errorString();
    }
}

void Server::clientConnected()
{
    QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
    DEBUG << "New connection: " << tcpSocket->peerAddress().toString() << ":" << tcpSocket->peerPort() << "...";

    connect (tcpSocket, &QTcpSocket::disconnected, this, &Server::clientDisconnected);
    connect (tcpSocket, &QTcpSocket::readyRead, this, &Server::incomingDataFromClient);

    ClientData client;
    client.tcpSocket = tcpSocket;
    client.incomeDataStream = new QDataStream();
    client.incomeDataStream->setDevice(tcpSocket);
    client.incomeDataStream->setVersion(QDataStream::Qt_6_7);
    clients.append(client);

    sendFilesList(client);

    DEBUG << "Current connected clients: " << clients.size();
}

void Server::clientDisconnected()
{
    QTcpSocket *tcpSocket = (QTcpSocket *)sender();

    for (int clientIndex = 0; clientIndex < clients.size(); clientIndex++) {
        auto &client = clients[clientIndex];
        if (client.tcpSocket != tcpSocket)
            continue;

        DEBUG << "Client " << client.tcpSocket->peerAddress().toString() << ":" << tcpSocket->peerPort() << " has been disconnected!";

        client.tcpSocket->close();
        delete client.tcpSocket;
        delete client.incomeDataStream;

        clients.removeAt(clientIndex);
        break;
    }

    DEBUG << "Current connected clients: " << clients.size();
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
