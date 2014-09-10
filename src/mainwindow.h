#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QFile>
#include "videowidget.h"
#include "jumpingslider.h"

#ifdef Q_OS_WIN
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QWinJumpList>
#include <QWinJumpListCategory>
#endif

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QTranslator *t,int argc, char *argv[], QWidget *parent = 0);

private slots:
    void on_actionOuvrir_triggered();
    void on_actionQuitter_triggered();
    void on_actionLecture_triggered(bool checked);
    void thumbnailLecture_triggered();
    void mediaDurationChanged(qint64 duration);
    void playerMetaDataChanged();
    void mediaChanged(int position);
    void mediaPositionChanged(qint64 position);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void bufferStatusChanged(int percentFilled);
    void seeked(int position);
    void on_actionPr_c_dent_triggered();
    void on_actionSuivant_triggered();
    void mediaStateChanged(QMediaPlayer::State state);
    void on_playlistTable_itemDoubleClicked(QListWidgetItem *item);
    void on_actionAjouter_triggered();
    void on_actionAl_atoire_triggered(bool checked);
    void playerVideoAvailable(bool available);
    void on_actionMode_triggered(bool checked);
    void on_actionStop_triggered();
    void on_actionImage_Suivante_triggered();
    void on_actionImage_pr_c_dente_triggered();
    void on_actionBoucle_triggered(bool checked);
    void playLeft();
    void playRight();
    void on_actionPr_f_rences_triggered();
    void on_actionA_Propos_triggered();
    void playerError(QMediaPlayer::Error error);
    void setVolume(int v);
    void mediaInserted(int start,int end);
    void mediaRemoved(int start,int end);

private:
    QString msToTC(qint64 ms); //converts milliseconds to String Timecode '00:00.00'
    void addFiles(QList<QUrl> fichiers); //adds some files to the playlist
    void addFile(QUrl fichier); //adds one file to the playlist
    QMediaPlayer *player; //Media player
    QMediaPlaylist *playlist; //playlist
    VideoWidget *video; //Video screen
    bool progra; //used to test if a button has been triggered programmatically
    QSize windowSize; //store the size of the main window when in full mode
    JumpingSlider *seekBar; //seekbar inherits QSlider but handles mouse event differently
    JumpingSlider *volumeSlider;
    qreal frameRate;
    QJsonObject getParams();
    bool buttonStyleAuto;
#ifdef Q_OS_WIN
    QWinTaskbarButton *taskbarButton;
    QWinTaskbarProgress *taskbarProgress;
#endif

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent*);
};

#endif // MAINWINDOW_H
