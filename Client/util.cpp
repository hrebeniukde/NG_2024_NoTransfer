#include "util.h"

QString Util::currentDirectoryPath = "";

void Util::sendWarningAlert(QString message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(message);
    msgBox.exec();
}

bool Util::isConnectedToServer(Network *network)
{
    bool connected = network->isConnectedToServer();

    if (!connected)
        sendWarningAlert("You are not connected to any server.");

    return connected;
}

QString Util::getItemParentDirectory(QString itemPath)
{
    return itemPath.left(itemPath.lastIndexOf('/'));
}

QString Util::getItemFullPath(QString itemName)
{
    return currentDirectoryPath + '/' + itemName;
}

QString Util::getItemNameFromPath(QString itemPath)
{
    return itemPath.mid(itemPath.lastIndexOf('/') + 1);
}
