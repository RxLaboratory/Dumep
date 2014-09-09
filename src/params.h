#ifndef PARAMS_H
#define PARAMS_H

#include "ui_params.h"
#include <QJsonDocument>
#include <QJsonObject>

class Params : public QDialog, private Ui::Params
{
    Q_OBJECT

public:
    explicit Params(QWidget *parent = 0);
    Qt::ToolButtonStyle getStyle();
    void setStyle(Qt::ToolButtonStyle style);
    bool getMime();
    void setMime(bool m);
    void setVolume(int v);
    int getVolume();
private slots:
    void on_comboBox_activated(int index);
    void on_buttonBox_accepted();

private:
    bool buttonStyleAuto;
    QJsonObject getParams();
    void setParams(QJsonObject p);
    int vol;
protected:
    void resizeEvent(QResizeEvent*);

};

#endif // PARAMS_H
