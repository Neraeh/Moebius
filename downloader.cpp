#include "downloader.h"

Downloader::Downloader(int verbose, QObject* _parent)
{
    parent = _parent;
    l = new Logger(verbose, this);
    downloading = false;
    l->log(INFO, tr("Initialized"));
}

Downloader::~Downloader()
{
    delete l;
}

bool Downloader::urlExists(QString url_string)
{
    l->log(INFO, tr("Checking %1...").arg(url_string));
    QUrl url(url_string);
    QTcpSocket socket;
    socket.connectToHost(url.host(), 80);
    if (socket.waitForConnected()) {
        socket.write("HEAD " + url.path().toUtf8() + " HTTP/1.1\r\n"
                     "Host: " + url.host().toUtf8() + "\r\n\r\n");
        if (socket.waitForReadyRead()) {
            QByteArray bytes = socket.readAll();
            if (bytes.contains("200 OK")) {
                l->log(INFO, tr("%1 : OK").arg(url_string));
                return true;
            }
        }
    }
    l->log(INFO, tr("%1 : not valid").arg(url_string));
    return false;
}

void Downloader::download(QString url_string)
{
    if (!downloading) {
        downloading = true;
        l->log(INFO, tr("Trying to download %1...").arg(url_string));
        QStringList splitted = url_string.split("/");
        name = splitted.last();
        QUrl url(url_string);
        QNetworkAccessManager* webCtrl = new QNetworkAccessManager(parent);
        QObject::connect(webCtrl, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
        QNetworkRequest request(url);
        webCtrl->get(request);
    }
}

void Downloader::finished(QNetworkReply *reply)
{
    downloading = false;

    if (reply->error() == QNetworkReply::NoError) {
        l->log(INFO, tr("%1 : downloaded").arg(name));
        emit downloadedData(name, reply->readAll());
    }
    else {
        l->log(INFO, tr("%1 : error - %2").arg(name).arg(reply->errorString()));
        emit error(name, reply->errorString());
    }

    reply->deleteLater();
}

bool Downloader::isDownloading()
{
    return downloading;
}
