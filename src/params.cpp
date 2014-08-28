#include "params.h"
#include <QMediaPlayer>

Params::Params(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    buttonStyleAuto = true;

}

void Params::on_comboBox_activated(int index)
{
    if (index == 0) buttonStyleAuto = true;
    if (index == 1)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    if (index == 2)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
    if (index == 3)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    if (index == 4)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
}

Qt::ToolButtonStyle Params::getStyle()
{
    Qt::ToolButtonStyle style = toolButton->toolButtonStyle();
    if (buttonStyleAuto == true) style = Qt::ToolButtonFollowStyle;
    return style;
}

void Params::setStyle(Qt::ToolButtonStyle style)
{

    if (style == Qt::ToolButtonIconOnly)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(1);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonTextOnly)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(2);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonTextBesideIcon)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(3);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonTextUnderIcon)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(4);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonFollowStyle)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(0);
        buttonStyleAuto = true;
    }
}

void Params::resizeEvent(QResizeEvent*)
{
    if (buttonStyleAuto)
    {
        if (this->width() > 420) toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        else if (this->width() > 380) toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        else toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}
