#ifndef STORYDOWNLOADER_H
#define STORYDOWNLOADER_H

#include <QObject>
#include "logger.h"
#include "moebius.h"
#include "downloader.h"

QT_FORWARD_DECLARE_CLASS(Moebius)
class StoryDownloader : public QObject
{
    Q_OBJECT
public:
    explicit StoryDownloader(int verbose, QString _name, Moebius *parent = nullptr);
    ~StoryDownloader();

public slots:
    void downloaded(QString part, QByteArray data);
    void downloadErr(QString part, QString error);

signals:
    void downloadError(QString file, QString error);
    void finished(QString name);

private:
    Logger* l;
    QString name;
    Downloader *story, *scenes, *choices;
};

#endif // STORYDOWNLOADER_H
