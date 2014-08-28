#ifndef JUMPINGSLIDER_H
#define JUMPINGSLIDER_H

#include <QSlider>

class JumpingSlider : public QSlider
{
    Q_OBJECT
public:
    explicit JumpingSlider(QWidget *parent = 0);

signals:

public slots:

protected:
    void mousePressEvent ( QMouseEvent * event );

};

#endif // JUMPINGSLIDER_H
