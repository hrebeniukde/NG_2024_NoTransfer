#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QMessageBox>

#include "network.h"

class Util
{
public:
    static void sendWarningAlert(QString message);

    static bool isConnectedToServer(Network *network);

    static QString getItemParentDirectory(QString itemPath);

    static QString getItemFullPath(QString itemName);

    static QString getItemNameFromPath(QString itemPath);

    static QString currentDirectoryPath;
};

#endif // UTIL_H
