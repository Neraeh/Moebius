#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>
#include <QCoreApplication>
#include "logger.h"
#include "moebius.h"
#include "storydownloader.h"

QT_FORWARD_DECLARE_CLASS(Moebius)
class Commands : public QObject
{
    Q_OBJECT
public:
    explicit Commands(int verbose, Moebius *_parent = nullptr);
    ~Commands();
    void exit(QString nick, QStringList args);
    void load(QString nick, QStringList args);
    void launch(QString nick, QStringList args);
    void download(QString nick, QStringList args);

private:
    Logger* l;
};

#endif // COMMANDS_H
