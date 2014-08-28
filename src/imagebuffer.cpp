#include "imagebuffer.h"
#include <QtDebug>

ImageBuffer::ImageBuffer(QObject *parent) :
    QThread(parent)
{
    bufferLength = 0;
    reverse = false;
    frame = 0;
}

void ImageBuffer::setFrames(QStringList f)
{
    frames = f;
}


void ImageBuffer::stop()
{
    this->quit();
}

void ImageBuffer::setReverse(bool r)
{
    reverse = r;
}

void ImageBuffer::setBufferLength(int l)
{
    bufferLength = l;
}

void ImageBuffer::run()
{
    qDebug() << "start buffering";
        while(true)
        {
            if (bufferLength < 76)
            {
                if (!reverse && frame < frames.count())
                {
                    QPixmap pict(frames[frame++]);
                    emit newFrame(&pict);
                    bufferLength++;
                }
                else if (frame > 0)
                {
                    QPixmap pict(frames[frame--]);
                    emit newFrame(&pict);
                    bufferLength++;
                }
                stateChanged(bufferLength*100/76);
            }
        }
}

