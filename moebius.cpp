#include "moebius.h"

Moebius::Moebius(QCoreApplication* _parent) : IrcConnection(_parent)
{
    qsrand(QTime::currentTime().msec());

    l = new Logger(0, this);
    l->log(INIT, tr("Initializing"));

    l->log(INIT, tr("Loading ini files"));

    settings = new QSettings("conf/settings.ini", QSettings::IniFormat);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    accesslist = new QSettings("conf/accesslist.ini", QSettings::IniFormat);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));

    if (settings->allKeys().isEmpty())
        l->log(WARNING, tr("settings.ini is empty or missing, using default values"));

    int verbose = settings->value("verbose", 2).toInt() % 4;
    if (verbose == 3)
        qputenv("IRC_DEBUG", "1");
    l->setLevel(verbose);
    l->log(INIT, tr("Verbose level set to %1").arg(QString::number(verbose)));

    trigger = settings->value("trigger", "!").toString();
    setHost(settings->value("server", "irc.freenode.net").toString());
    setPort(settings->value("port", 6667).toInt());
    setSecure(settings->value("ssl", false).toBool());
    chan = (settings->value("chan", "##ew").toString().startsWith("#") ? "" : "#") + settings->value("chan", "##newuno").toString();
    setEncoding(settings->value("encoding", "UTF-8").toByteArray());
    setUserName(settings->value("username", "M" + QString::number(qrand())).toString());
    setNickName(settings->value("nickname", userName()).toString());
    setRealName(settings->value("realname", userName()).toString());
    cnf = settings->value("commandnotfound", true).toBool();
    setReconnectDelay(5);

    l->log(INFO, tr("Server: %1 on port %2").arg(host()).arg(QString::number(port())));
    l->log(INFO, tr("SSL: %1").arg(isSecure() ? tr("true") : tr("false")));
    l->log(INFO, tr("Channel: %1").arg(chan));
    l->log(INFO, tr("Encoding: %1").arg(QString(encoding())));
    l->log(INFO, tr("Username: %1").arg(userName()));
    l->log(INFO, tr("Nickname: %1").arg(nickName()));
    l->log(INFO, tr("Realname: %1").arg(realName()));

    QVariantMap ctcp;
    ctcp.insert("VERSION", "Moebius alpha build");
    setCtcpReplies(ctcp);

    cmds = new Commands(verbose, this);
    commands = new QHash<QString,fp>;
    commands->insert("exit", &Commands::exit);

    QObject::connect(this, SIGNAL(secureError()), this, SLOT(onSSLError()));
    QObject::connect(this, SIGNAL(connected()), this, SLOT(onConnect()));
    QObject::connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    QObject::connect(this, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onIrcMessage(IrcMessage*)));
    QObject::connect(this, SIGNAL(privateMessageReceived(IrcPrivateMessage*)), this, SLOT(onMessage(IrcPrivateMessage*)));
    QObject::connect(this, SIGNAL(joinMessageReceived(IrcJoinMessage*)), this, SLOT(onJoin(IrcJoinMessage*)));
    QObject::connect(this, SIGNAL(kickMessageReceived(IrcKickMessage*)), this, SLOT(onKick(IrcKickMessage*)));
    QObject::connect(this, SIGNAL(modeMessageReceived(IrcModeMessage*)), this, SLOT(onMode(IrcModeMessage*)));
    QObject::connect(this, SIGNAL(nickMessageReceived(IrcNickMessage*)), this, SLOT(onNick(IrcNickMessage*)));
    QObject::connect(this, SIGNAL(noticeMessageReceived(IrcNoticeMessage*)), this, SLOT(onNotice(IrcNoticeMessage*)));
    QObject::connect(this, SIGNAL(partMessageReceived(IrcPartMessage*)), this, SLOT(onPart(IrcPartMessage*)));
    QObject::connect(this, SIGNAL(quitMessageReceived(IrcQuitMessage*)), this, SLOT(onQuit(IrcQuitMessage*)));

    l->log(INIT, tr("Moebius [alpha build] initialized"));
    l->log(INFO, tr("Connecting"));
    open();
}

Moebius::~Moebius()
{
    l->log(INFO, tr("Closing"));

    if (isActive()) {
        quit(tr("Closed"));
        close();
    }

    accesslist->sync();
    delete l;
    delete commands;
    delete settings;
    delete accesslist;
}

void Moebius::onSSLError()
{
    l->log(ERROR, tr("SSL error, set verbose level to 3 to show details"));
}

void Moebius::onConnect()
{
    if (!settings->value("nspassword", QString()).toString().isEmpty())
    {
        l->log(INFO, tr("Connected, logging in to NickServ"));
        sendCommand(IrcCommand::createMessage("NickServ", "IDENTIFY " + settings->value("nspassword").toString()));
    }
    else
    {
        l->log(INFO, tr("Connected, joining %1").arg(chan));
        sendCommand(IrcCommand::createJoin(chan));
    }
}

void Moebius::onDisconnect()
{
    l->log(WARNING, tr("Disconnected"));
}

void Moebius::onIrcMessage(IrcMessage *message)
{
    bool ok;
    int code = message->command().toInt(&ok);
    if (!ok)
        return;

    switch (code)
    {
    case 400:
        l->log(ERROR, tr("Unknown error reported by %1: %2").arg(host()).arg(message->parameters().join(" ")));
        break;
    case 403:
        l->log(ERROR, tr("Channel %1 does not exist, exiting").arg(chan));
        qApp->exit(403);
    case 404:
        l->log(ERROR, tr("Cannot send message to %1").arg(chan));
        break;
    case 431:
        l->log(ERROR, tr("%1 said nickname is empty, exiting").arg(host()));
        qApp->exit(431);
    case 432:
        l->log(ERROR, tr("Erroneous nickname, exiting"));
        qApp->exit(432);
    case 433:
        l->log(ERROR, tr("%1 is already in use, exiting").arg(nickName()));
        qApp->exit(433);
    case 436:
        l->log(ERROR, tr("Nickname collision, exiting"));
        qApp->exit(436);
    case 471:
        l->log(ERROR, tr("%1 is full, exiting").arg(chan));
        qApp->exit(471);
    case 473:
        l->log(ERROR, tr("Invite only chans are not yet supported, exiting"));
        qApp->exit(473);
    }
}

void Moebius::onMessage(IrcPrivateMessage* message)
{
    if (message->target() == nickName() || message->nick() == nickName())
        return;
    else if (message->content().startsWith(trigger)) {
        QStringList args = message->content().split(" ", QString::SkipEmptyParts);
        QString cmd = QString(args.first()).mid(1);
        args.removeFirst();
        command(message->nick(), cmd, args);
    }
}

void Moebius::onJoin(IrcJoinMessage* message)
{
    Q_UNUSED(message);
}

void Moebius::onKick(IrcKickMessage* message)
{
    if (message->user() == nickName())
        sendCommand(IrcCommand::createJoin(chan));
}

void Moebius::onMode(IrcModeMessage* message)
{
    if (!identified && message->target() == nickName() && message->mode().contains("+r")) {
        identified = true;
        l->log(INFO, tr("Identified. Joining %1").arg(chan));
        sendCommand(IrcCommand::createJoin(chan));
    }
}

void Moebius::onNick(IrcNickMessage* message)
{
    Q_UNUSED(message);
}

void Moebius::onNotice(IrcNoticeMessage* message)
{
    if (!identified && message->nick() == "NickServ" && (message->content().contains("identified") || message->content().contains("recognized"))) {
        identified = true;
        l->log(INFO, tr("Identified. Joining %1").arg(chan));
        sendCommand(IrcCommand::createJoin(chan));
    }
}

void Moebius::onPart(IrcPartMessage* message)
{
    Q_UNUSED(message);
}

void Moebius::onQuit(IrcQuitMessage* message)
{
    Q_UNUSED(message);
}

void Moebius::command(QString nick, QString cmd, QStringList args)
{
    if (commands->contains(cmd))
        (cmds->*commands->value(cmd))(nick, args);
    else if (cnf)
        sendCommand(IrcCommand::createNotice(nick, tr("This command does not exist")));
}

QSettings* Moebius::getAccessList()
{
    return accesslist;
}
