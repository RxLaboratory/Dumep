#ifndef PARAMS_H
#define PARAMS_H

#include "ui_params.h"

class Params : public QDialog, private Ui::Params
{
    Q_OBJECT

public:
    explicit Params(QWidget *parent = 0);
    Qt::ToolButtonStyle getStyle();
    void setStyle(Qt::ToolButtonStyle style);
private slots:
    void on_comboBox_activated(int index);
private:
    bool buttonStyleAuto;
protected:
    void resizeEvent(QResizeEvent*);

};

#endif // PARAMS_H
