#include "server.h"

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

        QString fileName;
        QByteArray fileData;
        *client.incomeDataStream >> fileName;
        *client.incomeDataStream >> fileData;

        if (!client.incomeDataStream->commitTransaction())
            return;

        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        file.write(fileData);
        file.close();

        DEBUG << "Client " << client.tcpSocket->peerAddress().toString() << ":" << tcpSocket->peerPort() << " has sent " << fileName << "...";
    }
}
