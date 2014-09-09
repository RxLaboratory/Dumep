#include "opener.h"
#include <QInputDialog>
#include <QUrl>
#include <QMessageBox>
#include <QtDebug>

Opener::Opener(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    //charger les favs
    QJsonArray favsArray = getFavs();
    for(int i = 0;i<favsArray.count();i++)
    {
        QJsonValue fav = favsArray[i];
        if (fav.toObject().value("type").toString() == "files")
        {
            QJsonArray url = fav.toObject().value("url").toArray();
            favsList->addItem(url[0].toString());
        }
        else if (fav.toObject().value("type").toString() == "folder")
        {
            favsList->addItem(fav.toObject().value("url").toString());
        }
        else if (fav.toObject().value("type").toString() == "stream")
        {
            favsList->addItem(fav.toObject().value("url").toString());
        }
        else if (fav.toObject().value("type").toString() == "imageSequence")
        {
            QJsonArray fs = fav.toObject().value("url").toArray();
            favsList->addItem(fs[0].toString());
        }
    }
    //charger les recents
    QJsonArray recentArray = getRecents();
    for(int i = 0;i<recentArray.count();i++)
    {
        QJsonValue recent = recentArray[i];
        if (recent.toObject().value("type").toString() == "files")
        {
            QJsonArray url = recent.toObject().value("url").toArray();
            recentList->addItem(url[0].toString());
        }
        else if (recent.toObject().value("type").toString() == "folder")
        {
            recentList->addItem(recent.toObject().value("url").toString());
        }
        else if (recent.toObject().value("type").toString() == "stream")
        {
            recentList->addItem(recent.toObject().value("url").toString());
        }
        else if (recent.toObject().value("type").toString() == "imageSequence")
        {
            QJsonArray fs = recent.toObject().value("url").toArray();
            recentList->addItem(fs[0].toString());
        }
    }

    //un thread pour parcourir les dossiers de facon récurrente
    bf = new BrowserThread();
    connect(bf,SIGNAL(nextFile(int)),this,SLOT(progress(int)));
    connect(bf,SIGNAL(numFiles(int)),this,SLOT(progressMax(int)));
    connect(bf,SIGNAL(finished()),this,SLOT(folderUrls()));
    progressBar->setStyleSheet("selection-background-color: rgb(255, 68, 68);");
    progressWidget->hide();


}

void Opener::folderUrls()
{
    openUrls = bf->getUrls();
    if (openUrls.count() > 0)
    {
        if (addToFavs->isChecked())
        {
            //charger les anciens favs
            QJsonArray favsArray = getFavs();

            QJsonObject newFavs;
            newFavs.insert("type","folder");
            newFavs.insert("url",bf->getFolder());
            favsArray.append(newFavs);

            setFavs(favsArray);
        }
        //recentlist
        //load recents
        QJsonArray recentArray = getRecents();

        QJsonObject newRecent;
        newRecent.insert("type","folder");
        newRecent.insert("url",bf->getFolder());
        recentArray.prepend(newRecent);
        if (recentArray.count() > 10)
        {
            recentArray.removeLast();
        }

        setRecent(recentArray);
        setCursor(Qt::ArrowCursor);
        buttonsWidget->setEnabled(true);
        favsList->setEnabled(true);
        accept();
    }
}

void Opener::progress(int i)
{
    progressBar->setValue(i);
}

void Opener::progressMax(int i)
{
    progressBar->setMaximum(i);


}

void Opener::setTitle(QString t)
{
    setWindowTitle(t);
}

void Opener::on_file_clicked()
{
    //demande les fichiers

        openUrls = QFileDialog::getOpenFileUrls(this,"Ouvrir des fichiers");
        if (openUrls.count() > 0)
        {
            if (addToFavs->isChecked())
            {
                //charger les anciens favs
                QJsonArray favsArray = getFavs();

                QJsonObject newFavs;
                newFavs.insert("type","files");
                QJsonArray fichiers;
                foreach(QUrl url,openUrls)
                {
                    fichiers.append(QJsonValue(url.toString()));
                }
                newFavs.insert("url",fichiers);
                favsArray.append(newFavs);

                setFavs(favsArray);
            }
            //recentlist
            //load recents
            QJsonArray recentArray = getRecents();

            QJsonObject newRecent;
            newRecent.insert("type","files");
            QJsonArray fichiers;
            foreach(QUrl url,openUrls)
            {
                fichiers.append(QJsonValue(url.toString()));
            }
            newRecent.insert("url",fichiers);
            recentArray.prepend(newRecent);
            if (recentArray.count() > 10)
            {
                recentArray.removeLast();
            }

            setRecent(recentArray);
            accept();
        }

}

void Opener::on_folder_clicked()
{

    QString dossier = QFileDialog::getExistingDirectory(this,"Ouvrir un dossier");
    if (dossier != "")
    {
        loadFolder(dossier);
    }
}

void Opener::on_stream_clicked()
{
    QString u = QInputDialog::getText(this,"Veuillez entrer l'url","URL du média :");
    if(u != "")
    {
        QUrl url(u);
        if (url.isValid())
        {
            if (addToFavs->isChecked())
            {
                //charger les anciens favs
                QJsonArray favsArray = getFavs();

                QJsonObject newFavs;
                newFavs.insert("type","stream");
                newFavs.insert("url",url.toString());
                favsArray.append(newFavs);

                setFavs(favsArray);
            }
            //recentlist
            //load recents
            QJsonArray recentArray = getRecents();

            QJsonObject newRecent;
            newRecent.insert("type","stream");
            newRecent.insert("url",url.toString());
            recentArray.prepend(newRecent);
            if (recentArray.count() > 10)
            {
                recentArray.removeLast();
            }
            setRecent(recentArray);

            openUrls << url;
            accept();
        }
        else
        {
            QMessageBox::warning(this,"URL invalide","Cette adresse  n'est pas valide.");
        }
    }
}

void Opener::loadFolder(QString dossier)
{
    progressBar->setValue(0);
    progressWidget->show();
    buttonsWidget->setEnabled(false);
    favsList->setEnabled(false);
    setCursor(Qt::WaitCursor);
    bf->setFolder(dossier);
    bf->start();
}

QJsonArray Opener::getFavs()
{
    QJsonDocument favsDoc;
    QString favsName;
    favsName = "/Dumep/favs.dumep";
    QFile favsFile(QDir::homePath() + favsName);
    if (favsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&favsFile);
        favsDoc = QJsonDocument::fromJson(in.readAll().toUtf8());
        favsFile.close();
    }
    if (favsDoc.isArray()) return favsDoc.array();
    else return QJsonArray();
}

QJsonArray Opener::getRecents()
{
    QJsonDocument recentDoc;
    QString recentName;
    recentName = "/Dumep/recent.dumep";
    QFile recentFile(QDir::homePath() + recentName);
    if (recentFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&recentFile);
        recentDoc = QJsonDocument::fromJson(in.readAll().toUtf8());
        recentFile.close();
    }
    if (recentDoc.isArray()) return recentDoc.array();
    else return QJsonArray();
}

void Opener::setRecent(QJsonArray recentArray)
{
    QJsonDocument recentDoc = QJsonDocument(recentArray);
    //écrire dans le fichier
    QString recentName;
    recentName = "/Dumep/recent.dumep";
    QFile recentFile(QDir::homePath() + recentName);
    if (recentFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&recentFile);
        out << recentDoc.toJson();
        recentFile.close();
    }
}

void Opener::setFavs(QJsonArray favsArray)
{
    QJsonDocument favsDoc = QJsonDocument(favsArray);
    //écrire dans le fichier
    QString favsName;
    favsName = "/Dumep/favs.dumep";
    QFile favsFile(QDir::homePath() + favsName);
    if (favsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&favsFile);
        out << favsDoc.toJson();
        favsFile.close();
    }
}

QList<QUrl> Opener::getUrls()
{
    return openUrls;
}

void Opener::on_favsList_itemDoubleClicked(QListWidgetItem *item)
{
    QJsonArray favs = getFavs();
    QJsonObject open = favs.at(favsList->currentRow()).toObject();
    if (open.value("type").toString() == "files")
    {
        QJsonArray urls = open.value("url").toArray();
        for (int i = 0;i<urls.count();i++)
        {
            openUrls << QUrl(urls[i].toString());
        }
        accept();
    }
    else if (open.value("type").toString() == "folder")
    {
        loadFolder(open.value("url").toString());
    }
    else
    {
        openUrls << QUrl(open.value("url").toString());
        accept();
    }
}

void Opener::on_recentList_itemDoubleClicked(QListWidgetItem *item)
{
    QJsonArray recent = getRecents();
    QJsonObject open = recent.at(recentList->currentRow()).toObject();
    if (open.value("type").toString() == "files")
    {
        QJsonArray urls = open.value("url").toArray();
        for (int i = 0;i<urls.count();i++)
        {
            openUrls << QUrl(urls[i].toString());
        }
        accept();
    }
    else if (open.value("type").toString() == "folder")
    {
        loadFolder(open.value("url").toString());
    }
    else
    {
        openUrls << QUrl(open.value("url").toString());
        accept();
    }
}


void Opener::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        if (favsList->hasFocus())
        {
            while(favsList->selectedItems().count() > 0)
            {
                int row = favsList->row(favsList->selectedItems().first());
                QListWidgetItem *item = favsList->takeItem(row);
                QJsonArray favs;
                favs = getFavs();
                favs.removeAt(row);
                setFavs(favs);
                delete item;
            }
        event->accept();
        }
    }
    else if (event->key() == Qt::Key_Return)
    {
        if (favsList->hasFocus())
        {
            QJsonArray favs = getFavs();
            foreach(QListWidgetItem *item,favsList->selectedItems())
            {
                QJsonObject open = favs.at(favsList->row(item)).toObject();
                if (open.value("type").toString() == "files")
                {
                    QJsonArray urls = open.value("url").toArray();
                    for (int i = 0;i<urls.count();i++)
                    {
                        openUrls << QUrl(urls[i].toString());
                    }
                    accept();
                }
                else if (open.value("type").toString() == "folder")
                {
                    loadFolder(open.value("url").toString());
                }
                else
                {
                    openUrls << QUrl(open.value("url").toString());
                    accept();
                }
            }
        }
        else if (recentList->hasFocus())
        {
            QJsonArray recent = getRecents();
            foreach(QListWidgetItem *item,recentList->selectedItems())
            {
                QJsonObject open = recent.at(recentList->row(item)).toObject();
                if (open.value("type").toString() == "files")
                {
                    QJsonArray urls = open.value("url").toArray();
                    for (int i = 0;i<urls.count();i++)
                    {
                        openUrls << QUrl(urls[i].toString());
                    }
                    accept();
                }
                else if (open.value("type").toString() == "folder")
                {
                    loadFolder(open.value("url").toString());
                }
                else
                {
                    openUrls << QUrl(open.value("url").toString());
                    accept();
                }
            }
        }
        event->accept();
    }
    else
    {
        QDialog::keyPressEvent(event);
    }
}

void Opener::on_cancelButton_clicked()
{
    bf->terminate();
    progressWidget->hide();
    progressBar->setValue(0);
    buttonsWidget->setEnabled(true);
    favsList->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

