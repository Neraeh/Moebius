#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QTcpSocket>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "logger.h"

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader(int verbose, QObject* _parent);
    ~Downloader();
    bool urlExists(QString url_string);
    void download(QString url_string);
    bool isDownloading();

private slots:
    void finished(QNetworkReply* reply);

signals:
    void downloadedData(QString file, QByteArray data);
    void error(QString file, QString error);

private:
    QObject* parent;
    Logger* l;
    bool downloading;
    QString name;
};

#endif // DOWNLOADER_H
