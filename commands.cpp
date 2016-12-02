#include "commands.h"

Commands::Commands(int verbose, Moebius* _parent) : QObject(_parent)
{
    parent = _parent;
    l = new Logger(verbose, this);
    l->log(INFO, tr("Initialized"));
}

Commands::~Commands()
{
    delete l;
}

void Commands::exit(QString nick, QStringList args)
{
    Q_UNUSED(nick)
    Q_UNUSED(args)

    if (parent->inAccessList(nick)) {
        l->log(INFO, tr("Closing (%1)").arg(nick));
        qApp->quit();
    }
    else
        l->log(INFO, tr("%1 tried to close me. Eck!").arg(nick));
}

void Commands::benchmark(QString nick, QStringList args)
{
    Q_UNUSED(nick)
    Q_UNUSED(args)

    QTime timer;
    timer.start();
    parent->getReader()->load("template");
    int time = timer.elapsed();

    Story story = parent->getReader()->getLoadedStory();
    parent->sendMessage(parent->getChan(),
                        "Loaded " + story.getName() + " version " + story.getVersion() + " in " + QString::number(time) + "msecs");
}
