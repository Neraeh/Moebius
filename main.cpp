#include <QTranslator>
#include "moebius.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    if (a.arguments().contains("--version") || a.arguments().contains("-v"))
    {
        qDebug() << qPrintable("Moebius [alpha build]");
        return 0;
    }

    QSettings* settings = new QSettings(qApp->applicationDirPath() + "/conf/settings.ini", QSettings::IniFormat);
    settings->setIniCodec("UTF-8");
    QString lang = settings->value("language", "en").toString();
    QTranslator translator;
    translator.load("tr/" + lang);
    a.installTranslator(&translator);

    Moebius moebius(&a);
    return a.exec();
}
