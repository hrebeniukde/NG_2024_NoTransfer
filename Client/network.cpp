#include "network.h"

#include "logger.h"

Network::Network(QObject *parent)
    : QObject{parent}
{
    tcpSocket = new QTcpSocket();

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
    // TODO: Implement networking
}
