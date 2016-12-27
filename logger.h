#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QObject>

enum Log
{
    INFO,
    WARNING,
    ERROR,
    INIT
};

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger(int _verbose, QObject* _parent);

    void log(Log level, QString message);
    void setLevel(int level);
    int getLevel();

private:
    int verbose;
    QString parent;
};

#endif // LOGGER_H
