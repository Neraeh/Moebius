#ifndef MOEBIUS_H
#define MOEBIUS_H

#include <IrcConnection>
#include <IrcCommand>
#include <QCoreApplication>
#include <QStringList>
#include <QSettings>
#include <QTextCodec>
#include "logger.h"
#include "commands.h"

QT_FORWARD_DECLARE_CLASS(Commands)
class Moebius : public IrcConnection
{
    Q_OBJECT
public:
    Moebius(QCoreApplication* _parent = nullptr);
    ~Moebius();
    QSettings* getAccessList();

private:
    typedef void (Commands::*fp)(QString, QStringList);

private slots:
    void onSSLError();
    void onConnect();
    void onDisconnect();
    void onIrcMessage(IrcMessage* message);
    void onMessage(IrcPrivateMessage* message);
    void onJoin(IrcJoinMessage* message);
    void onKick(IrcKickMessage* message);
    void onMode(IrcModeMessage* message);
    void onNick(IrcNickMessage* message);
    void onNotice(IrcNoticeMessage* message);
    void onPart(IrcPartMessage* message);
    void onQuit(IrcQuitMessage* message);

private:
    void command(QString nick, QString cmd, QStringList args);

private:
    Logger* l;
    QHash<QString,fp>* commands;
    Commands *cmds;
    QSettings *settings, *accesslist;
    unsigned int verbose;
    QString chan, trigger;
    bool identified, cnf;
};

#endif // MOEBIUS_H
