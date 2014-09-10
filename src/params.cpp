#include "params.h"
#include <QMediaPlayer>
#include <QFile>
#include <QDir>
#include <QFileDialog>


Params::Params(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    buttonStyleAuto = true;

    QJsonObject params = getParams();


    Qt::ToolButtonStyle style = Qt::ToolButtonStyle(params.value("buttonStyle").toInt());
    bool m = params.value("checkMimeType").toBool();
    vol = params.value("volume").toInt();
    languageFile = params.value("language").toString();
    if (params.value("volume").isUndefined()) vol = 100;
    lastBrowsed = params.value("lastBrowsed").toString();

    setStyle(style);
    setMime(m);
    setLanguage(languageFile);

}

//buttons
void Params::on_buttonBox_accepted()
{
    QJsonObject params;
    params.insert("buttonStyle",getStyle());
    params.insert("checkMimeType",getMime());
    params.insert("language",languageFile);
    params.insert("volume",vol);
    params.insert("lastBrowsed",lastBrowsed);
    setParams(params);
    accept();
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

void Params::on_languageBox_activated(int index)
{
    if (index == 0) languageFile = "languages/dumep_fr";
    else if (index == 1) languageFile = "languages/dumep_en";
    else
    {
        QString l = QFileDialog::getOpenFileName(this,tr("Choisir un fichier de traduction"),QString(),tr("Fichier de langue (*.qm)"));
        if (l!="")
        {
            languageFile = l;
        }
    }
}

//setting and getting params

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

bool Params::getMime()
{
    return mimeButton->isChecked();
}

void Params::setMime(bool m)
{
    mimeButton->setChecked(m);
}

void Params::setVolume(int v)
{
    vol = v;
    on_buttonBox_accepted();
}

int Params::getVolume()
{
    return vol;
}

void Params::setLanguage(QString l)
{
    if (l == "languages/dumep_en") languageBox->setCurrentIndex(1);
    else if (l == "languages/dumep_fr") languageBox->setCurrentIndex(0);
    else languageBox->setCurrentIndex(2);
}

QString Params::getLanguage()
{
    return languageFile;
}

void Params::setLastBrowsed(QString l)
{
    lastBrowsed = l;
    on_buttonBox_accepted();
}

QString Params::getLastBrowsed()
{
    return lastBrowsed;
}

//loading and saving params

QJsonObject Params::getParams()
{
    //charger
    QJsonDocument paramsDoc;
    QFile paramsFile(QDir::homePath() + "/Dumep/params.dumep");
    if (paramsFile.exists())
    {
        if (paramsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&paramsFile);
            paramsDoc = QJsonDocument::fromJson(in.readAll().toUtf8());
            paramsFile.close();
        }
    }
    else
    {
        //enregistrer
        QJsonObject params;
        params.insert("buttonStyle",Qt::ToolButtonFollowStyle);
        params.insert("checkMimeType",false);
        params.insert("volume",100);
        params.insert("lastBrowsed","");
        params.insert("language","languages/dumep_" + QLocale::system().name());
        paramsDoc.setObject(params);
        if (paramsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&paramsFile);
            out << paramsDoc.toJson();
            paramsFile.close();
        }
    }
    return paramsDoc.object();
}

void Params::setParams(QJsonObject p)
{
    QJsonDocument paramsDoc;
    paramsDoc.setObject(p);
    QFile paramsFile(QDir::homePath() + "/Dumep/params.dumep");
    //enregistrer
    if (paramsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&paramsFile);
        out << paramsDoc.toJson();
        paramsFile.close();
    }
}

//events

void Params::resizeEvent(QResizeEvent*)
{
    if (buttonStyleAuto)
    {
        if (this->width() > 450) toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        else if (this->width() > 420) toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        else toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}

