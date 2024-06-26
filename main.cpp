#include <QApplication>
#include <QTranslator>
#include <ctime>
#include <cstdio>
#include <cstdlib>

#include "FenetrePrincipale.h"
#include "FenetreCouleurs.h"
#include "FenetreGenerateurs.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    auto locale{QLocale::system().name().section('_', 0, 0)};

    if (locale != QString("fr") && locale != QString("en"))
        locale = QString("en");

    QTranslator translator;
    translator.load(QString(":/Langages/qt_") + locale);
    application.installTranslator(&translator);
    QTranslator translatorBase;
    translatorBase.load(QString(":/Langages/qtbase_") + locale);
    application.installTranslator(&translatorBase);
    QTranslator translatorSubstrate;
    translatorSubstrate.load(QString(":/Langages/Substrate_") + locale);
    application.installTranslator(&translatorSubstrate);

    srand(time(NULL));

    FenetrePrincipale fenetrePrincipale;

    return application.exec();
}
