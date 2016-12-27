#include "storydownloader.h"

StoryDownloader::StoryDownloader(int verbose, QString _name, Moebius *parent) : QObject(parent)
{
    l = new Logger(verbose, this);
    name = _name;

    story = new Downloader(verbose, this);
    scenes = new Downloader(verbose, this);
    choices = new Downloader(verbose, this);

    QString baseurl = parent->getRepoUrl();

    if (story->urlExists(baseurl + name + "/story.json") && scenes->urlExists(baseurl + name + "/scenes.json") && choices->urlExists(baseurl + name + "/story.json")) {
        QObject::connect(story, SIGNAL(downloadedData(QString,QByteArray)), this, SLOT(downloaded(QString,QByteArray)));
        QObject::connect(story, SIGNAL(error(QString,QString)), this, SLOT(downloadErr(QString,QString)));
        QObject::connect(scenes, SIGNAL(downloadedData(QString,QByteArray)), this, SLOT(downloaded(QString,QByteArray)));
        QObject::connect(scenes, SIGNAL(error(QString,QString)), this, SLOT(downloadErr(QString,QString)));
        QObject::connect(choices, SIGNAL(downloadedData(QString,QByteArray)), this, SLOT(downloaded(QString,QByteArray)));
        QObject::connect(choices, SIGNAL(error(QString,QString)), this, SLOT(downloadErr(QString,QString)));

        story->download(baseurl + name + "/story.json");
        scenes->download(baseurl + name + "/scenes.json");
        choices->download(baseurl + name + "/choices.json");
    }
    else {
        l->log(ERROR, tr("%1 does not exist").arg(baseurl + name));
        emit downloadErr(name, tr("%1 does not exist").arg(name));
    }
}

StoryDownloader::~StoryDownloader()
{
    delete l;
    delete story;
    delete scenes;
    delete choices;
}

void StoryDownloader::downloaded(QString part, QByteArray data)
{
    QDir dest("downloaded");
    if (!dest.exists()) {
        if (!dest.mkpath("."))
            l->log(ERROR, tr("Unable to create downloaded directory"));
    }

    dest.mkpath(name);
    QFile file("downloaded/" + name + "/" + part);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        l->log(ERROR, tr("Unable to open %1 in write mode: %2").arg(part).arg(file.errorString()));
        emit downloadError(part, file.errorString());
        return;
    }

    file.write(data);
    l->log(INFO, tr("%1 : saved").arg(name + "/" + part));

    if (!story->isDownloading() && !scenes->isDownloading() && !choices->isDownloading())
    {
        l->log(INFO, tr("%1 downloaded").arg(name));
        emit finished(name);
        this->deleteLater();
    }
}

void StoryDownloader::downloadErr(QString part, QString error)
{
    emit downloadError(part, error);
    this->deleteLater();
}
