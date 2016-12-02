#include "logger.h"

Logger::Logger(int _verbose, QObject *_parent) : QObject(_parent)
{
    verbose = _verbose;
    parent = _parent->metaObject()->className();
}

void Logger::log(Log level, QString message)
{
    switch(level) {
    case INFO:
        if (verbose >= 2)
            qDebug() << qPrintable(QString("[%1] %2: %3").arg(tr("INFO"), parent, message));
        break;
    case WARNING:
        if (verbose != 0)
            qWarning() << qPrintable(QString("[%1] %2: %3").arg(tr("WARN"), parent, message));
        break;
    case ERROR:
        qCritical() <<  qPrintable(QString("[%1] %2: %3").arg(tr("ERROR"), parent, message));
        break;
    case INIT:
        qDebug() << qPrintable(message);
    }
}

void Logger::setLevel(int level)
{
    verbose = level;
}
