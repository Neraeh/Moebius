#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>
#include <QCoreApplication>
#include "logger.h"
#include "moebius.h"

QT_FORWARD_DECLARE_CLASS(Moebius)
class Commands : public QObject
{
    Q_OBJECT
public:
    explicit Commands(int verbose, Moebius *_parent = nullptr);
    void exit(QString nick, QStringList args);

private:
    Logger* l;
    Moebius* parent;
};

#endif // COMMANDS_H
