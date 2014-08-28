#include "browser.h"
#include <QDir>
#include <QUrl>

Browser::Browser(QObject *parent) :
    QObject(parent)
{
}

QList<QUrl> Browser::browseFolder(QString dossier)
{
    QList<QUrl> trouve;
    QDir d(dossier);
    //d.setFilter(QDir::NoDotAndDotDot);
    QFileInfoList dedans = d.entryInfoList(QStringList("*"),QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    //TODO regarde si ya une séquence d'images dedans avant d'ajouter (dans ce cas on laisse tomber les fichiers trouvés dedans

    foreach(QFileInfo f,dedans)
    {
        if (f.isDir())
        {
            trouve.append(browseFolder(f.absoluteFilePath()));
        }
        else
        {
            trouve.append(QUrl::fromLocalFile(f.absoluteFilePath()));
        }
    }
    return trouve;
}
