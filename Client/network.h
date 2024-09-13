#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>

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

private slots:
    void tcpSocketConnected();

    void tcpSocketDisconnected();

    void tcpSocketReadyRead();

signals:
    void updateFilesList(QVector<ServerFile> filesList, QString directoryPath);

private:
    QTcpSocket *tcpSocket;

    QDataStream incomeDataStream;

    void handleIncomingRpc(int rpcId);
};

#endif // NETWORK_H
