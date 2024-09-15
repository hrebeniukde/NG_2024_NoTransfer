#include "util.h"

QString Util::getItemParentDirectory(QString itemPath)
{
    return itemPath.left(itemPath.lastIndexOf('/'));
}
