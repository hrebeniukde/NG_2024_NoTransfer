#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QFile>
#include <QDir>

#define DEBUG qDebug().noquote().nospace()

#define SHARED_FOLDER_PATH "Shared" // do not place '/' in the end of the path

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

    void handleIncomingRpc(ClientData client, int rpcId);

    void sendFilesList(ClientData client, QString directoryPath = "");

    void sendFile(ClientData client, QString filePath, bool isTempFile);

    void sendUploadFinished(ClientData client);

    QString getClientFormattedAddress(QTcpSocket *client);
};

#endif // SERVER_H
