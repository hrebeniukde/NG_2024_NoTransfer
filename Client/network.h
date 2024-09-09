#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = nullptr);
    ~Network();

    void connectToServer(QHostAddress host, int port);

    bool isAlreadyConnected(QHostAddress host, int port);

private slots:
    void tcpSocketConnected();

    void tcpSocketDisconnected();

    void tcpSocketReadyRead();

signals:

private:
    QTcpSocket *tcpSocket;
};

#endif // NETWORK_H
