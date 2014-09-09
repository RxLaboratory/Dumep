#include "about.h"

About::About(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    QString version = "1.0 %1 %2";
    QString texteVersion = versionLabel->text() + version;
#ifdef Q_OS_WIN64
    texteVersion = texteVersion.arg("Win");
    texteVersion = texteVersion.arg("64bit");
#else
#ifdef Q_OS_WIN32
    texteVersion = texteVersion.arg("Win");
    texteVersion = texteVersion.arg("32bit");
#endif
#endif
#ifdef Q_OS_LINUX
    texteVersion = texteVersion.arg("Linux");
#ifdef Q_PROCESSOR_X86_32
    texteVersion = texteVersion.arg("32bit");
#endif
#ifdef Q_PROCESSOR_X86_64
    texteVersion = texteVersion.arg("64bit");
#endif
#endif
#ifdef Q_OS_MAC
    texteVersion = texteVersion.arg("MacOS");
#ifdef Q_PROCESSOR_X86_32
    texteVersion = texteVersion.arg("32bit");
#endif
#ifdef Q_PROCESSOR_X86_64
    texteVersion = texteVersion.arg("64bit");
#endif
#endif
    texteVersion = texteVersion.replace("%1","").replace("%2","");
    versionLabel->setText(texteVersion);

}
