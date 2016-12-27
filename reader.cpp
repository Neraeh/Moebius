#include "reader.h"

Reader::Reader(int verbose, QObject *parent) : QObject(parent)
{
    l = new Logger(verbose, this);

    story = nullptr;

    l->log(INFO, tr("Initialized"));
}

Reader::~Reader()
{
    delete l;
}

bool Reader::load(QString file)
{
    QFile storyFile("storys/" + file + "/story.json");
    QFile scenesFile("storys/" + file + "/scenes.json");
    QFile choicesFile("storys/" + file + "/choices.json");

    if (!storyFile.open(QIODevice::ReadOnly | QIODevice::Text))  {
        l->log(ERROR, tr("Unable to open %1: %2").arg(file).arg(storyFile.errorString()));
        return false;
    }
    else if (!scenesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        l->log(ERROR, tr("Unable to open %1: %2").arg(file).arg(scenesFile.errorString()));
        return false;
    }
    else if (!choicesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        l->log(ERROR, tr("Unable to open %1: %2").arg(file).arg(choicesFile.errorString()));
        return false;
    }

    QByteArray storyData = storyFile.readAll();
    QByteArray scenesData = scenesFile.readAll();
    QByteArray choicesData = choicesFile.readAll();

    storyFile.close();
    scenesFile.close();
    choicesFile.close();


    QJsonParseError* err = new QJsonParseError;

    QJsonDocument storyDoc = QJsonDocument::fromJson(storyData, err);
    if (err->error != 0) {
        l->log(ERROR, tr("Unable to parse JSON: %1").arg(err->errorString()));
        return false;
    }
    else if (storyDoc.isNull()) {
        l->log(ERROR, tr("Invalid JSON: %1").arg(file + "/story.json"));
        return false;
    }
    else if (storyDoc.isEmpty()) {
        l->log(ERROR, tr("%1 is empty").arg(file + "/story.json"));
        return false;
    }

    QJsonDocument scenesDoc = QJsonDocument::fromJson(scenesData, err);
    if (err->error != 0) {
        l->log(ERROR, tr("Unable to parse JSON: %1").arg(err->errorString()));
        return false;
    }
    else if (storyDoc.isNull()) {
        l->log(ERROR, tr("Invalid JSON: %1").arg(file + "/scenes.json"));
        return false;
    }
    else if (storyDoc.isEmpty()) {
        l->log(ERROR, tr("%1 is empty").arg(file + "/scenes.json"));
        return false;
    }

    QJsonDocument choicesDoc = QJsonDocument::fromJson(choicesData, err);
    if (err->error != 0) {
        l->log(ERROR, tr("Unable to parse JSON: %1").arg(err->errorString()));
        return false;
    }
    else if (storyDoc.isNull()) {
        l->log(ERROR, tr("Invalid JSON: %1").arg(file + "/choices.json"));
        return false;
    }
    else if (storyDoc.isEmpty()) {
        l->log(ERROR, tr("%1 is empty").arg(file + "/choices.json"));
        return false;
    }


    if (storyDoc.isObject() && scenesDoc.isObject() && choicesDoc.isObject()) {
        l->log(INFO, tr("Loading %1").arg(file));

        QJsonObject storyObject = storyDoc.object();
        QJsonObject scenesObject = scenesDoc.object();
        QJsonObject choicesObject = choicesDoc.object();
        QVariantMap storyMap = storyObject.toVariantMap();
        QVariantMap scenesMap = scenesObject.toVariantMap();
        QVariantMap choicesMap = choicesObject.toVariantMap();
        QHash<QString,QStringList> scenes;
        QHash<QString,QHash<QString,QString>> choices;

        story = new Story(storyMap["name"].toString(),
                storyMap["desc"].toStringList(),
                storyMap["author"].toString(),
                storyMap["version"].toString(),
                storyMap["genre"].toInt(),
                storyMap["opensource"].toBool(),
                storyMap["website"].toString(),
                storyMap["start"].toString());

        for (QVariantMap::const_iterator iter = scenesMap.begin(); iter != scenesMap.end(); ++iter)
            scenes.insert(iter.key(), iter.value().toStringList());

        for (QVariantMap::const_iterator iter = choicesMap.begin(); iter != choicesMap.end(); ++iter) {
            QHash<QString,QString> value;
            QStringList content = iter.value().toStringList();
            foreach (QString w, content) {
                QString to = w.split("=>").last().trimmed();
                QStringList keys = w.split("=>").first().split(",");
                foreach (QString key, keys)
                    value.insert(key.trimmed(), to);
            }
            choices.insert(iter.key(), value);
        }

        l->log(INFO, tr("Done"));
        return true;
    }
    else if (storyDoc.isArray())
        l->log(ERROR, tr("%1 is not a valid story file").arg(file));
    else if (scenesDoc.isArray())
        l->log(ERROR, tr("%1 is not a valid scenes file").arg(file));
    else if (choicesDoc.isArray())
        l->log(ERROR, tr("%1 is not a valid choices file").arg(file));
    else
        l->log(ERROR, tr("Unable to determine type of JSON: %1").arg(file));

    return false;
}

Story Reader::getLoadedStory()
{
    return *story;
}

bool Reader::isLoaded()
{
    return story != nullptr;
}
