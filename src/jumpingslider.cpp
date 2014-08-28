#include "jumpingslider.h"
#include <QStyleOptionSlider>
#include <QMouseEvent>

JumpingSlider::JumpingSlider(QWidget *parent) :
    QSlider(parent)
{

}


void JumpingSlider::mousePressEvent ( QMouseEvent * event )
{
  QStyleOptionSlider opt;
  initStyleOption(&opt);
  QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

  if (event->button() == Qt::LeftButton && sr.contains(event->pos()) == false)
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
  }
  QSlider::mousePressEvent(event);
}
