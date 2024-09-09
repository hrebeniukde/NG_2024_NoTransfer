#ifndef LOGGER_H
#define LOGGER_H

#include <QTextEdit>

class Logger
{
public:
    static void initLogger(QTextEdit *textEdit);

    static void printLog(QString message);

    static void clearLogs();

private:
    static QTextEdit *logTextEdit;
};

#endif // LOGGER_H
