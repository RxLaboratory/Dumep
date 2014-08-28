#ifndef BROWSER_H
#define BROWSER_H

#include <QObject>

class Browser : public QObject
{
    Q_OBJECT
public:
    explicit Browser(QObject *parent = 0);
    static QList<QUrl> browseFolder(QString dossier);

signals:

public slots:



};

#endif // BROWSER_H
