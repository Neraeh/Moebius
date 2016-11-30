#include "commands.h"

Commands::Commands(int verbose, Moebius* _parent) : QObject(_parent)
{
    parent = _parent;
    l = new Logger(verbose, this);
}

void Commands::exit(QString nick, QStringList args)
{
    Q_UNUSED(nick);
    Q_UNUSED(args);

    if (parent->getAccessList()->contains(nick)) {
        l->log(INFO, tr("Closing (%1)").arg(nick));
        qApp->quit();
    }
    else
        l->log(INFO, tr("%1 tried to close me. Eck!").arg(nick));
}
