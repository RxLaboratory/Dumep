#ifndef BROWSERTHREAD_H
#define BROWSERTHREAD_H

#include <QThread>
#include <QDir>
#include <QUrl>

class BrowserThread : public QThread
{
    Q_OBJECT
public:
    explicit BrowserThread(QObject *parent = 0);
    void run();
    void setFolder(QString d);
    QString getFolder();
    QList<QUrl> getUrls();

signals:
    void numFiles(int);
    void nextFile(int);
    void finished();

public slots:

private:
    QString folder;
    QList<QUrl> browseFolder(QString d);
    QList<QUrl> urls;

};

#endif // BROWSERTHREAD_H
