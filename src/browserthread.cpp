#include "browserthread.h"


BrowserThread::BrowserThread(QObject *parent) :
    QThread(parent)
{
}

void BrowserThread::setFolder(QString d)
{
    folder = d;
}

QString BrowserThread::getFolder()
{
    return folder;
}

void BrowserThread::run()
{
    QDir dir(folder);
    //d.setFilter(QDir::NoDotAndDotDot);
    QFileInfoList dedans = dir.entryInfoList(QStringList("*"),QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    emit numFiles(dedans.count());
    int i = 0;

    foreach(QFileInfo f,dedans)
    {
        emit nextFile(i);
        i++;
        if (f.isDir())
        {
            urls.append(browseFolder(f.absoluteFilePath()));
        }
        else
        {
            urls.append(QUrl::fromLocalFile(f.absoluteFilePath()));
        }
    }
    emit finished();
}

QList<QUrl> BrowserThread::getUrls()
{
    return urls;
}

QList<QUrl> BrowserThread::browseFolder(QString d)
{
    QList<QUrl> trouve;
    QDir dir(d);
    //d.setFilter(QDir::NoDotAndDotDot);
    QFileInfoList dedans = dir.entryInfoList(QStringList("*"),QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);


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
