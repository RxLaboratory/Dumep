#ifndef JUMPINGSLIDER_H
#define JUMPINGSLIDER_H

#include <QProgressBar>

class JumpingSlider : public QProgressBar
{
    Q_OBJECT
public:
    explicit JumpingSlider(QWidget *parent = 0);

signals:

public slots:

protected:
    void mouseMoveEvent ( QMouseEvent * event );

};

#endif // JUMPINGSLIDER_H
