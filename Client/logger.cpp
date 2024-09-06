#include "logger.h"

#include <QDateTime>

Logger::Logger(QTextEdit *textEdit)
{
    logTextEdit = textEdit;
}

void Logger::printLog(QString message)
{
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("dd.MM.yyyy hh:mm:ss");

    logTextEdit->append(
        QString("<strong>[%1]</strong> %2").arg(formattedTime, message)
    );
}

void Logger::clearLogs()
{
    logTextEdit->clear();
}
