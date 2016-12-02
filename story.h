#ifndef STORY_H
#define STORY_H

#include <QStringList>
#include <QHash>

class Story
{
public:
    explicit Story(QString _name,
                   QStringList _description,
                   QString _author,
                   QString _version,
                   int _genre,
                   bool _opensource,
                   QString _website,
                   QString _start);
    void setScenes(QHash<QString,QStringList> _scenes);
    void setChoices(QHash<QString,QHash<QString,QString>> _choices);
    QString getName();
    QStringList getDescription();
    QString getAuthor();
    QString getVersion();
    int getGenre();
    bool getOpensource();
    QString getWebsite();
    QString getStart();

private:
    QHash<QString,QStringList> scenes;
    QHash<QString,QHash<QString,QString>> choices;
    QString name, author, version, website, start;
    QStringList description;
    int genre;
    bool opensource;
};

#endif // STORY_H
