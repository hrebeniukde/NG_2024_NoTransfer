#include <QCoreApplication>

#include "server.h"

#define SERVER_HOST "0.0.0.0"
#define SERVER_PORT 2024

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
    server.start(SERVER_HOST, SERVER_PORT);

    return a.exec();
}
