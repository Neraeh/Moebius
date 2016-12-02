#include "story.h"

Story::Story(QString _name, QStringList _description, QString _author, QString _version, int _genre, bool _opensource, QString _website, QString _start)
{
    name = _name;
    description = _description;
    author = _author;
    version = _version;
    genre = _genre;
    opensource = _opensource;
    website = _website;
    start = _start;
}

void Story::setScenes(QHash<QString,QStringList> _scenes)
{
    scenes = _scenes;
}

void Story::setChoices(QHash<QString,QHash<QString,QString>> _choices)
{
    choices = _choices;
}

QString Story::getName()
{
    return name;
}

QStringList Story::getDescription()
{
    return description;
}

QString Story::getAuthor()
{
    return author;
}

QString Story::getVersion()
{
    return version;
}

int Story::getGenre()
{
    return genre;
}

bool Story::getOpensource()
{
    return opensource;
}

QString Story::getWebsite()
{
    return website;
}

QString Story::getStart()
{
    return start;
}
