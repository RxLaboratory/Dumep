#include "jumpingslider.h"
#include <QStyleOptionSlider>
#include <QMouseEvent>
#include <QStyleFactory>

JumpingSlider::JumpingSlider(QWidget *parent) :
    QProgressBar(parent)
{
    setTextVisible(false);
    setStyle(QStyleFactory::create("Fusion"));
}


void JumpingSlider::mouseMoveEvent ( QMouseEvent * event )
{

    int newVal;
    if (orientation() == Qt::Vertical)
       newVal = minimum() + ((maximum()-minimum()) * (height()-event->y())) / height();
    else
       newVal = minimum() + ((maximum()-minimum()) * event->x()) / width();

    if (invertedAppearance() == true)
        setValue( maximum() - newVal );
    else
        setValue(newVal);

    event->accept();

  QProgressBar::mouseMoveEvent(event);
}

