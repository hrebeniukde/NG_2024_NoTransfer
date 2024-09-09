#include "logger.h"

#include <QDateTime>

QTextEdit *Logger::logTextEdit = nullptr;

void Logger::initLogger(QTextEdit *textEdit)
{
    Logger::logTextEdit = textEdit;
}

void Logger::printLog(QString message)
{
    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("dd.MM.yyyy hh:mm:ss");

    Logger::logTextEdit->append(
        QString("<strong>[%1]</strong> %2").arg(formattedTime, message)
    );
}

void Logger::clearLogs()
{
    Logger::logTextEdit->clear();
}
