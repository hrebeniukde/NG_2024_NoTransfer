#ifndef LOGGER_H
#define LOGGER_H

#include <QTextEdit>

class Logger
{
public:
    Logger(QTextEdit *textEdit);

    void printLog(QString message);

    void clearLogs();

private:
    QTextEdit *logTextEdit;
};

#endif // LOGGER_H
