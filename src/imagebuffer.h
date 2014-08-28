#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QThread>
#include <QStringList>
#include <QPixmap>

class ImageBuffer : public QThread
{
    Q_OBJECT
public:
    explicit ImageBuffer(QObject *parent = 0);
    void setFrames(QStringList f);
    void run();

signals:
    void stateChanged(int);
    void newFrame(QPixmap *);

public slots:
    void stop();
    void setReverse(bool r);
    void setBufferLength(int l);

private:
    QStringList frames;
    bool reverse;
    int frame;
    int bufferLength;
};

#endif // IMAGEBUFFER_H
