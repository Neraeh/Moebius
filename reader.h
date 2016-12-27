#ifndef READER_H
#define READER_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QHash>
#include "moebius.h"
#include "story.h"
#include "logger.h"

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(int verbose, QObject *parent = nullptr);
    ~Reader();
    bool load(QString file);
    Story getLoadedStory();
    bool isLoaded();

private:
    Logger* l;
    Story* story;
};

#endif // READER_H
