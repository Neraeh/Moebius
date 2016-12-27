#include "commands.h"

Commands::Commands(int verbose, Moebius* _parent) : QObject(_parent)
{
    l = new Logger(verbose, this);
    l->log(INFO, tr("Initialized"));
}

Commands::~Commands()
{
    delete l;
}

void Commands::exit(QString nick, QStringList args)
{
    Q_UNUSED(args)

    Moebius *parent = qobject_cast<Moebius*>(this->parent());

    if (parent->inAccessList(nick)) {
        l->log(INFO, tr("Closing (%1)").arg(nick));
        qApp->quit();
    }
    else
        l->log(INFO, tr("%1 tried to close me. Eck!").arg(nick));
}

void Commands::load(QString nick, QStringList args)
{
    Moebius *parent = qobject_cast<Moebius*>(this->parent());

    if (!parent->inAccessList(nick))
        return;

    QString file;
    if (args.isEmpty())
        file = "template";
    else
        file = args.at(0);

    QTime timer;
    timer.start();
    bool ok = parent->getReader()->load(file);
    int time = timer.elapsed();

    if (ok) {
        Story story = parent->getReader()->getLoadedStory();
        parent->sendMessage(parent->getChan(),
                            "Loaded " + story.getName() + " version " + story.getVersion() + " in " + QString::number(time) + "msecs");
    }
    else {
        parent->sendMessage(parent->getChan(),
                            "Error, " + file + " could not be loaded");
    }
}

void Commands::launch(QString nick, QStringList args)
{
    Q_UNUSED(nick)
    Q_UNUSED(args)

    Moebius *parent = qobject_cast<Moebius*>(this->parent());

    if (!parent->getReader()->isLoaded()) {
        parent->sendMessage(parent->getChan(), tr("You must load a story before using !launch"));
        return;
    }

    parent->setInGame(true);
}

void Commands::download(QString nick, QStringList args)
{
    Moebius *parent = qobject_cast<Moebius*>(this->parent());

    if (!parent->inAccessList(nick))
        return;

    if (args.length() != 1) {
        parent->sendMessage(parent->getChan(), tr("Only one argument required"));
        return;
    }

    QString name = args.at(0);
    parent->sendMessage(parent->getChan(), tr("Trying to download %1...").arg(name));

    StoryDownloader *downloader = new StoryDownloader(l->getLevel(), name, parent);
    QObject::connect(downloader, SIGNAL(finished(QString)), parent, SLOT(onDowloaded(QString)));
    QObject::connect(downloader, SIGNAL(downloadError(QString,QString)), parent, SLOT(onDownloadError(QString,QString)));
}
