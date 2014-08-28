#ifndef OPENER_H
#define OPENER_H

#include "ui_opener.h"
#include <QFile>
#include <QJsonArray>
#include <QFileDialog>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>

class Opener : public QDialog, private Ui::Opener
{
    Q_OBJECT

public:
    explicit Opener(QWidget *parent = 0);
    QList<QUrl> getUrls();
    void setTitle(QString t);
private slots:
    void on_file_clicked();
    void on_folder_clicked();
    void on_stream_clicked();
    void on_favsList_itemDoubleClicked(QListWidgetItem *item);

private:
    QList<QUrl> openUrls;
    QFile *favsFile;
    QJsonArray getFavs();
    void setFavs(QJsonArray favsArray);
protected:
    virtual void keyPressEvent(QKeyEvent *event);

};

#endif // OPENER_H
