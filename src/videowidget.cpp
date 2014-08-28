#include "videowidget.h"

#include <QKeyEvent>
#include <QMouseEvent>

VideoWidget::VideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    setPalette(p);

    setAttribute(Qt::WA_OpaquePaintEvent);

    connect(this,SIGNAL(fullScreenChanged(bool)),this,SLOT(onFullScreenChanged(bool)));
}

void VideoWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen()) {
        setFullScreen(false);
        event->accept();
    } else if (event->key() == Qt::Key_Enter && event->modifiers() & Qt::Key_Alt) {
        setFullScreen(!isFullScreen());
        event->accept();
    } else {
        QVideoWidget::keyPressEvent(event);
    }
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}

void VideoWidget::mousePressEvent(QMouseEvent *event)
{
    QVideoWidget::mousePressEvent(event);
}

void VideoWidget::onFullScreenChanged(bool fullScreen)
{
    if (fullScreen) this->setCursor(QCursor(Qt::BlankCursor));
    else this->setCursor(QCursor(Qt::ArrowCursor));
}
