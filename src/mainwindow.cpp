#include "mainwindow.h"
#include <QMimeData>
#include <QDropEvent>
#include <QAudioDeviceInfo>
#include <QDir>
#include <QNetworkRequest>
#include <stdlib.h>
#include <QMediaMetaData>
#include <QTimer>
#include <QMovie>
#include "opener.h"
#include "params.h"
#include "about.h"
#include <QMessageBox>
#include <QMimeDatabase>
#include <QMimeType>
#include "browserthread.h"


MainWindow::MainWindow(QTranslator *t,int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent)
{
    progra = true;
    setupUi(this);

    ctrlPressed = false;

    //widgets and players
    video = new VideoWidget(this);
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    seekBar = new JumpingSlider;
    seekBar->setOrientation(Qt::Horizontal);
    seekBar->setMaximumHeight(10);
    seekBar->setStyleSheet("selection-background-color: rgb(255, 68, 68);");
    volumeSlider = new JumpingSlider;
    volumeSlider->setOrientation(Qt::Horizontal);
    volumeSlider->setMaximumHeight(10);
    volumeSlider->setMaximumWidth(75);
    volumeSlider->setAlignment(Qt::AlignRight);
    volumeSlider->setStyleSheet("selection-background-color: rgb(137, 31, 31);");
    playControlsLayout->insertWidget(2,seekBar);
    volumeLayout->insertWidget(0,volumeSlider);
    frameRate = 25;

    //configuring media player
    player->setPlaylist(playlist);
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(mediaDurationChanged(qint64)));
    connect(playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(mediaChanged(int)));
    connect(playlist,SIGNAL(mediaInserted(int,int)),this,SLOT(mediaInserted(int,int)));
    connect(playlist,SIGNAL(mediaRemoved(int,int)),this,SLOT(mediaRemoved(int,int)));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(mediaPositionChanged(qint64)));
    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(setVolume(int)));
    connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(player,SIGNAL(bufferStatusChanged(int)),this,SLOT(bufferStatusChanged(int)));
    connect(player,SIGNAL(videoAvailableChanged(bool)),this,SLOT(playerVideoAvailable(bool)));
    connect(player,SIGNAL(metaDataChanged()),this,SLOT(playerMetaDataChanged()));
    connect(player,SIGNAL(error(QMediaPlayer::Error)),this,SLOT(playerError(QMediaPlayer::Error)));
    player->setNotifyInterval(1000/frameRate);
    connect(seekBar,SIGNAL(valueChanged(int)),this,SLOT(seeked(int)));
    videoScreenLayout->addWidget(video);
    player->setVideoOutput(video);

    //showing only the playlist
    splitter->setSizes(QList<int>() << 0 << 200);
    searchField->hide();

    progra = false;

    this->setAcceptDrops(true);

    //adding a folder in the home folder for preferences and favs
    QDir::home().mkdir("Dumep");

    buttonStyleAuto = true;

    //charge les préférences
    Params p(this);
    Qt::ToolButtonStyle style = p.getStyle();
    if (style == Qt::ToolButtonFollowStyle) buttonStyleAuto = true;
    else
    {
        buttonStyleAuto = false;
        mainToolBar->setToolButtonStyle(style);
    }

    volumeSlider->setValue(p.getVolume());

    //Ajouter les contrôles dans la TaskBar si win
#ifdef Q_OS_WIN

    //Jumplist
    QWinJumpList jumplist;
    jumplist.recent()->setVisible(true);
    jumplist.frequent()->setVisible(true);

    //buttons
   QWinThumbnailToolBar *thumbbar = new QWinThumbnailToolBar(this);
   thumbbar->setWindow(this->windowHandle());

   QWinThumbnailToolButton *stop = new QWinThumbnailToolButton(thumbbar);
   stop->setToolTip("Stop");
   stop->setIcon(QIcon(":/icones/stop"));
   stop->setDismissOnClick(true);
   stop->setFlat(true);
   connect(stop, &QWinThumbnailToolButton::clicked, this, &MainWindow::on_actionStop_triggered);

   QWinThumbnailToolButton *previous = new QWinThumbnailToolButton(thumbbar);
   previous->setToolTip("Précédent");
   previous->setIcon(QIcon(":/icones/previous"));
   previous->setDismissOnClick(false);
   previous->setFlat(true);
   connect(previous, &QWinThumbnailToolButton::clicked, this, &MainWindow::on_actionPr_c_dent_triggered);

   QWinThumbnailToolButton *play = new QWinThumbnailToolButton(thumbbar);
   play->setToolTip("Lecture");
   play->setIcon(QIcon(":/icones/play"));
   play->setDismissOnClick(true);
   play->setFlat(true);
   connect(play, &QWinThumbnailToolButton::clicked, this, &MainWindow::thumbnailLecture_triggered);

   QWinThumbnailToolButton *next = new QWinThumbnailToolButton(thumbbar);
   next->setToolTip("Suivant");
   next->setIcon(QIcon(":/icones/next"));
   next->setDismissOnClick(true);
   next->setFlat(true);
   connect(next, &QWinThumbnailToolButton::clicked, this, &MainWindow::on_actionSuivant_triggered);

   thumbbar->addButton(stop);
   thumbbar->addButton(previous);
   thumbbar->addButton(play);
   thumbbar->addButton(next);

   //progress
   taskbarButton = new QWinTaskbarButton(this);
   taskbarButton->setWindow(windowHandle());

   taskbarProgress = taskbarButton->progress();
   connect(seekBar, SIGNAL(valueChanged(int)), taskbarProgress, SLOT(setValue(int)));

#endif //Q_OS_WIN


    //load arguments
    if (argc > 1)
    {
        QList<QUrl> charge;
        for(int i = 1;i<argc;i++)
        {
            QUrl f = QUrl::fromLocalFile(argv[i]);
            charge.append(f);
        }
        addFiles(charge);
        player->play();
        actionLecture->setChecked(true);
    }


}

//USEFUL

QString MainWindow::msToTC(qint64 ms)
{
    //convertir en minutes:secondes.images
    int minutes = ms/1000.0/60.0;
    int seconds = (ms-minutes*60.0*1000.0)/1000.0;
    int images = (ms-minutes*60.0*1000.0-seconds*1000.0)*frameRate/1000.0;

    QString m = QString::number(minutes);
    QString s = QString::number(seconds);
    QString i = QString::number(images);
    minutes < 10 ? m = "0" + m : m;
    seconds < 10 ? s = "0" + s : s;
    images < 10 ? i = "0" + i : i;
    return m + ":" + s + "." + i;
}

void MainWindow::addFiles(QList<QUrl> fichiers)
{
    if (fichiers.count() == 0) return;
    foreach(QUrl fichier,fichiers)
    {
        addFile(fichier);
    }
}

void MainWindow::addFile(QUrl fichier)
{
    if (fichier.isLocalFile())
    {
        if (fichier.toString().toLower().endsWith(".m3u"))
        {
            playlist->load(fichier);
        }
        else
        {
            playlist->addMedia(fichier);
        }
    }
    else
    {
        if(fichier.toString().toLower().endsWith(".m3u"))playlist->load(QNetworkRequest(fichier));
        else playlist->addMedia(QMediaContent(QNetworkRequest(fichier)));
    }
}

void MainWindow::setVolume(int v)
{
    player->setVolume(v);
    volumeSpinBox->setValue(v);
    Params p(this);
    p.setVolume(v);
    repaint();
}

//BUTTONS

void MainWindow::on_actionOuvrir_triggered()
{

        Opener o(this);
        o.setTitle(tr("Ouvrir"));
        if (o.exec())
        {
            //vider la playlist et le tableau
            bool playing = player->state() == QMediaPlayer::PlayingState;
            player->stop();
            playlist->clear();
            //ajoute
            addFiles(o.getUrls());
            if (playing) player->play();
        }

}

void MainWindow::on_actionAjouter_triggered()
{
    //ajoute
    Opener o(this);
    o.setTitle("Ajouter");
    if (o.exec())
    {
        addFiles(o.getUrls());
    }
}

void MainWindow::on_actionQuitter_triggered()
{
    player->stop();
    this->close();
}

void MainWindow::on_actionLecture_triggered(bool checked)
{
    if (progra) return;

        if (playlist->currentIndex() < 0 && playlist->mediaCount() > 0) playlist->setCurrentIndex(0);
        if (checked && playlist->mediaCount() > 0)
        {
            player->play();
        }
        else if (checked)
        {
            actionLecture->setChecked(false);
        }
        else
        {
            player->pause();
        }

}

void MainWindow::thumbnailLecture_triggered()
{
    if (player->state() != QMediaPlayer::PlayingState) on_actionLecture_triggered(true);
    else on_actionLecture_triggered(false);
}

void MainWindow::on_actionPr_c_dent_triggered()
{
    if (player->position() < 2000)
    {
        playlist->previous();
    }
    else
    {
        seekBar->setValue(0);
    }
}

void MainWindow::on_actionSuivant_triggered()
{
    playlist->next();
}

void MainWindow::on_playlistTable_itemDoubleClicked(QListWidgetItem *item)
{
    playlist->setCurrentIndex(playlistTable->currentRow());

    player->play();
}

void MainWindow::on_actionAl_atoire_triggered(bool checked)
{
    if (checked) playlist->setPlaybackMode(QMediaPlaylist::Random);
    else if (actionBoucle->isChecked()) playlist->setPlaybackMode(QMediaPlaylist::Loop);
    else playlist->setPlaybackMode(QMediaPlaylist::Sequential);
}

void MainWindow::on_actionBoucle_triggered(bool checked)
{
    if (checked) playlist->setPlaybackMode(QMediaPlaylist::Loop);
    else if (actionAl_atoire->isChecked()) playlist->setPlaybackMode(QMediaPlaylist::Random);
    else playlist->setPlaybackMode(QMediaPlaylist::Sequential);
}

void MainWindow::on_actionMode_triggered(bool checked)
{
    if (checked)
    {
        this->showNormal();
        windowSize = this->size();

        splitter->hide();
        this->resize(QSize(580,0));
    }
    else
    {
        splitter->show();
        this->resize(windowSize);

    }
}

void MainWindow::on_actionStop_triggered()
{
    currentTCLabel->setText("Stoppé");
    playControls->setEnabled(false);
    player->stop();
}

void MainWindow::on_actionImage_Suivante_triggered()
{
    if (!player->isVideoAvailable()) return;


    player->pause();
    int frameDuration = 1000/frameRate;
    seekBar->setValue(player->position()+frameDuration);

}

void MainWindow::on_actionImage_pr_c_dente_triggered()
{ 
    if (!player->isVideoAvailable()) return;

    player->pause();
    int frameDuration = 1000/frameRate;
    seekBar->setValue(player->position()-frameDuration);

}

void MainWindow::on_actionPr_f_rences_triggered()
{
    Params p(this);

    if (p.exec())
    {
        Qt::ToolButtonStyle style = p.getStyle();

        if (style == Qt::ToolButtonFollowStyle)
        {
            buttonStyleAuto = true;
            QResizeEvent re(this->size(),this->size());
            resizeEvent(&re);
        }
        else
        {
            buttonStyleAuto = false;
            mainToolBar->setToolButtonStyle(style);
        }

    }
}

void MainWindow::on_actionA_Propos_triggered()
{
    About about(0);
    about.exec();
}

void MainWindow::on_search_textEdited(const QString &arg1)
{
    if (playlistTable->count() == 0) return;
    searchResult = playlistTable->findItems(arg1,Qt::MatchContains);
    if (searchResult.count() > 0)
    {
        playlistTable->setCurrentItem(searchResult[0]);
        currentSearchResult = 0;
    }
}

void MainWindow::on_searchPrevious_clicked()
{
    if (searchResult.count() < 1) return;
    if (currentSearchResult > 0 && searchResult.count() > currentSearchResult)
    {
        playlistTable->setCurrentItem(searchResult[currentSearchResult-1]);
        currentSearchResult--;
    }
    else
    {
        playlistTable->setCurrentItem(searchResult[searchResult.count()-1]);
        currentSearchResult = searchResult.count()-1;
    }
}

void MainWindow::on_searchNext_clicked()
{
    if (searchResult.count() < 1) return;
    if (currentSearchResult < searchResult.count()-1)
    {
        playlistTable->setCurrentItem(searchResult[currentSearchResult+1]);
        currentSearchResult++;
    }
    else
    {
        playlistTable->setCurrentItem(searchResult[0]);
        currentSearchResult = 0;
    }
}

void MainWindow::on_searchButton_toggled(bool checked)
{
    if (checked) searchField->show();
    else searchField->hide();
}



//PLAYER SLOTS

void MainWindow::mediaChanged(int position)
{
    if (progra) return;

    //virer le gras du tableau
    QFont font = playlistTable->item(0)->font();
    font.setBold(false);
    for (int i = 0;i<playlistTable->count();i++)
    {
        playlistTable->item(i)->setFont(font);
    }


    if (position >= 0 && position < playlistTable->count())
    {
        font.setBold(true);
        playlistTable->item(position)->setFont(font);
        setWindowTitle(playlistTable->item(position)->text());
    }
    else
    {
        setWindowTitle("Duduf Media Player");
        setWindowIcon(QIcon(":/icones/stop"));
    }
}

void MainWindow::mediaInserted(int start,int end)
{
    for (int i = start; i<= end;i++)
    {
        playlistTable->insertItem(i,playlist->media(i).canonicalUrl().fileName());
    }
}

void MainWindow::mediaRemoved(int start ,int end)
{
    for (int i = end; i>= start;i--)
    {
        QListWidgetItem *item = playlistTable->takeItem(i);
        delete item;
    }
}

void MainWindow::playerMetaDataChanged()
{
    //chopper les metadatas si y en a
    if (!player->isMetaDataAvailable()) return;
    QString tout = "";
    tout = player->metaData("Current").toString();
    tout += player->metaData("CurrentlyPlaying").toString();
    tout += player->metaData("CurrentTitle").toString();

    if (tout == "") tout = player->metaData("Director").toString();
    if (tout == "") tout = player->metaData("Author").toStringList().join(", ");
    if (tout == "") tout = player->metaData("Title").toString();
    else if (player->metaData("Title").toString() != "") tout += " - " + player->metaData("Title").toString();
    if (tout != "") setWindowTitle(tout);

    frameRate = player->metaData("VideoFrameRate").toReal();
    if (frameRate > 1) player->setNotifyInterval(1000/frameRate);
    else player->setNotifyInterval(1000);
}

void MainWindow::mediaDurationChanged(qint64 duration)
{
    if (duration == 0)
    {
        playControls->setEnabled(false);
        durationLabel->setText("--:--");
    }
    else
    {
        durationLabel->setText(this->msToTC(duration));
        playControls->setEnabled(true);
    }
    seekBar->setMaximum(duration);
#ifdef Q_OS_WIN
    taskbarProgress->setMaximum(duration);
#endif
}

void MainWindow::mediaPositionChanged(qint64 position)
{
    progra = true;
    seekBar->setValue(position);
    currentTCLabel->setText(this->msToTC(position));
   repaint();
    progra = false;
}

void MainWindow::seeked(int position)
{
    if (progra) return;

    currentTCLabel->setText(this->msToTC(position));
    player->setPosition(seekBar->value());
    repaint();
}

void MainWindow::mediaStateChanged(QMediaPlayer::State state)
{

    progra = true;
    if (state == QMediaPlayer::PausedState)
    {
        actionLecture->setChecked(false);
        this->setWindowIcon(QIcon(":/icones/pause"));
    }
    else if (state == QMediaPlayer::StoppedState)
    {
        actionLecture->setChecked(false);
        playControls->setEnabled(false);
        this->setWindowIcon(QIcon(":/icones/stop"));
    }
    else if (state == QMediaPlayer::PlayingState)
    {
        actionLecture->setChecked(true);
        this->setWindowIcon(QIcon(":/icones/play"));
    }
    progra = false;
}

void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::UnknownMediaStatus)
    {
        playControls->setEnabled(true);
        currentTCLabel->setText("");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::NoMedia)
    {
        playControls->setEnabled(false);
        currentTCLabel->setText("Rien à lire");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::LoadingMedia)
    {
        playControls->setEnabled(true);
        currentTCLabel->setText("Chargement...");
        this->setCursor(QCursor(Qt::BusyCursor));
    }
    if (status == QMediaPlayer::LoadedMedia)
    {
        playControls->setEnabled(true);
        currentTCLabel->setText("00:00");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::StalledMedia)
    {
        playControls->setEnabled(true);
        currentTCLabel->setText("Mise en mémoire tampon...");
        this->setCursor(QCursor(Qt::BusyCursor));
    }
    if (status == QMediaPlayer::BufferingMedia)
    {
        playControls->setEnabled(true);
        currentTCLabel->setText("Mise en mémoire tampon...");
        this->setCursor(QCursor(Qt::BusyCursor));
    }
    if (status == QMediaPlayer::BufferedMedia)
    {
        playControls->setEnabled(true);
        currentTCLabel->setText("Lecture en streaming");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::EndOfMedia)
    {
        playControls->setEnabled(false);
        currentTCLabel->setText("Rien à lire");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::InvalidMedia)
    {
        //hack:
        //check if we're playing: if true, the player goes one index too far, need one previous
        bool ps = player->state() == QMediaPlayer::PlayingState;

        if (playlist->playbackMode() != QMediaPlaylist::Sequential) player->stop();

       QMimeDatabase mdb;
       QString mimeName = mdb.mimeTypeForUrl(playlist->currentMedia().canonicalUrl()).name();
       QString mimeComment = mdb.mimeTypeForUrl(playlist->currentMedia().canonicalUrl()).comment();
       QMessageBox::warning(this,"Erreur","Problème de lecture : format de média non supporté.\n\nFormat : " + mimeName + "\nDescription : " + mimeComment);

        //QMessageBox::information(this,"Erreur de format","Format de média non supporté.");

        if (ps)
        {
            playlist->previous();
        }

        playControls->setEnabled(false);
        currentTCLabel->setText("Média invalide");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::bufferStatusChanged(int percentFilled)
{
      bufferSpinBox->setValue(percentFilled);
}

void MainWindow::playerVideoAvailable(bool available)
{
    if (available)
    {
        video->show();
        splitter->setSizes(QList<int>() << 500 << 200);
        actionImage_Suivante->setEnabled(true);
        actionImage_pr_c_dente->setEnabled(true);
    }
    else
    {
        video->hide();
        splitter->setSizes(QList<int>() << 0 << 200);
        actionImage_Suivante->setEnabled(false);
        actionImage_pr_c_dente->setEnabled(false);
    }
}

void MainWindow::playLeft()
{
        player->play();
        player->setPlaybackRate(-1);

}

void MainWindow::playRight()
{
    player->play();
}

void MainWindow::playerError(QMediaPlayer::Error error)
{
    if (error == QMediaPlayer::NoError)
    {
        return;
    }

    if (error == QMediaPlayer::ResourceError)
    {
        QMessageBox::warning(this,"Erreur","Lecture impossible : média introuvable");
    }
    else if (error == QMediaPlayer::FormatError)
    {
        //playing is stopped in mediaStatusChanged : invalid media
    }
    else if (error == QMediaPlayer::NetworkError) QMessageBox::information(this,"Erreur","Problème d'accès réseau, vérifiez votre connexion internet et recommencez la lecture un peu plus tard.");
    else if (error == QMediaPlayer::AccessDeniedError) QMessageBox::warning(this,"Erreur","Lecture impossible : droits d'accès au média insuffisants.");
    else if (error == QMediaPlayer::ServiceMissingError)
    {
#ifdef Q_OS_WIN
        QMessageBox::warning(this,"Erreur","Lecture impossible : aucun service de lecture multimédia disponible.\nMicrosoft DirectShow est-il disponible sur votre système ?");
#endif
#ifdef Q_OS_MAC
        QMessageBox::warning(this,"Erreur","Lecture impossible : aucun service de lecture multimédia disponible.\nQuicktime est-il correctement installé ?");
#endif
#ifdef Q_OS_LINUX
        QMessageBox::warning(this,"Erreur","Lecture impossible : aucun service de lecture multimédia disponible.\nAvez-vous installé GStreamer ?");
#endif

    }



}

//DRAG AND DROP

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        foreach(QUrl f,urlList)
        {
            if (QDir(f.toLocalFile()).exists())
            {
                BrowserThread bt;
                addFiles(bt.browseFolder(f.toLocalFile()));
            }
            else
            {
                addFile(f);
            }
        }
    }
    else if (mimeData->hasText())
    {
        if (QFile(mimeData->text()).exists())
        {
                QUrl f = QUrl::fromLocalFile(mimeData->text());
                QList<QUrl> fichiers;
                fichiers << f;
                addFiles(fichiers);
        }
        else if (QDir(mimeData->text()).exists())
        {
            BrowserThread bt;
            addFiles(bt.browseFolder(mimeData->text()));
        }
    }

    event->acceptProposedAction();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
     event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

//KEYBOARD

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        if (!actionMode->isChecked())
        {
            foreach(QListWidgetItem *item,playlistTable->selectedItems())
            {
                playlist->removeMedia(playlistTable->row(item));
            }
            event->accept();
        }
    }
    else if (event->key() == Qt::Key_Right)
    {
        if (!event->isAutoRepeat())
        {
            on_actionImage_Suivante_triggered();
        }
        else
        {
            playRight();
        }
        event->accept();
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (!event->isAutoRepeat())
        {
            on_actionImage_pr_c_dente_triggered();
        }
        else
        {
            playLeft();
        }
        event->accept();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right)
    {
        if (!event->isAutoRepeat())
        {
            on_actionLecture_triggered(false);
        }
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (!event->isAutoRepeat())
        {
            player->setPlaybackRate(1);
            on_actionLecture_triggered(false);
        }
    }
    else
    {
        QMainWindow::keyReleaseEvent(event);
    }
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    if (buttonStyleAuto)
    {
        int max = 1270;
        int min = 750;
#ifdef Q_OS_LINUX
        max = 1500;
        min = 990;
#endif
        if (this->width() > max) mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        else if (this->width() > min) mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        else mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}

