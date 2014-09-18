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
    void setLanguage(QString l);
    QString getLanguage();
    void setLastBrowsed(QString l);
    QString getLastBrowsed();
    void setNumRecents(int n);
    int getNumRecents();
private slots:
    void on_comboBox_activated(int index);
    void on_buttonBox_accepted();
    void on_languageBox_activated(int index);

private:
    bool buttonStyleAuto;
    QJsonObject getParams();
    void setParams(QJsonObject p);
    int vol;
    QString languageFile;
    QString lastBrowsed;
protected:
    void resizeEvent(QResizeEvent*);

};

#endif // PARAMS_H
