#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QFile>

#define DEBUG qDebug().noquote().nospace()

struct ClientData
{
    QTcpSocket *tcpSocket;
    QDataStream *incomeDataStream;
};

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

public slots:
    void start(QString host, int port);

private slots:
    void clientConnected();
    void clientDisconnected();
    void incomingDataFromClient();

private:
    QTcpServer *tcpServer;

    QVector<ClientData> clients;
};

#endif // SERVER_H
