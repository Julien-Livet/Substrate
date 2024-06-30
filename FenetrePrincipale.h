#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QImage>
#include <QMainWindow>
#include <QPrinter>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#include "FancyLineEdit.h"
#include "Interpolation.h"

#define VERSION QString("2.0")

class Substrate;
class WidgetSubstrate;
class FenetreCouleurs;
class FenetreGenerateurs;

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT

    public:
        FenetrePrincipale(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
        ~FenetrePrincipale();
        void enregistrerFichier(QString fichier);
        void chargerFichier(QString fichier);
        void chargerSubstrate(Substrate *s) const;
        void enregistrer(QDataStream &out) const;
        void charger(QDataStream &in);

    public slots:
        void genererAnnuler();
        bool quitter();
        void enregistrer();
        void enregistrerSous();
        void ouvrir();
        void aPropos();

    private:
        FenetreCouleurs *fenetreCouleurs;
        FenetreGenerateurs *fenetreGenerateurs;
        Substrate *substrateGenere;
        WidgetSubstrate *widgetSubstrate;
        QImage *imageSubstrate;
        QPrinter *printerSubstrate;
        QSpinBox *spinBoxNombreCracksInitial;
        QSpinBox *spinBoxNombreCracksFinal;
        QSpinBox *spinBoxNombreCracksSimultanesEnCreation;
        QSpinBox *spinBoxProbabiliteDedoublementCrack;
        QRadioButton *radioBoutonSable;
        QSpinBox *spinBoxNombreGrains;
        QRadioButton *radioBoutonFilDeFer;
        QCheckBox *checkBoxRemplissageZones;
        QSpinBox *spinBoxPourcentageCercles;
        Interpolation interpolation;
        QSpinBox *spinBoxLongueur;
        QSpinBox *spinBoxHauteur;
        QDoubleSpinBox *spinBoxFacteurEchelle;
        QRadioButton *radioBoutonFenetre;
        QCheckBox *checkBoxPleinEcran;
        QSpinBox *spinBoxPeriodeCroissance;
        QRadioButton *radioBoutonImage;
        FancyLineEdit *fancyLineEditFichier;
        QCheckBox *checkBoxNumeroIntegre;
        QSpinBox *spinBoxNumero;
        QRadioButton *radioBoutonImprimante;
        QPushButton *boutonConfigurerImprimante;
        QComboBox *comboBoxResolutionImprimante;
        QCheckBox *checkBoxAdaptationFormat;
        QComboBox *comboBoxPrioriteThread;
        QCheckBox *checkBoxAntialiasingActive;
        QCheckBox *checkBoxRepassageFinalCracks;
        QProgressBar *barreDeProgression;
        QPushButton *boutonGenererAnnuler;
        int longueur;
        int hauteur;
        qreal facteurEchelle;
        uint compteurSubstrates;
        QString nomFichier;
        int choixSortie;

    private slots:
        void montrerFenetreCouleurs();
        void montrerFenetreGenerateurs();
        void montrerFenetreInterpolation();
        void parcourirFichierImage();
        void substrateTermine();
        void configurerImprimante();
        void nombreCracksInitialChange(int v);
        void nombreCracksFinalChange(int v);
        void afficherMessageRemplissage();

    protected:
        void closeEvent(QCloseEvent *event);
};

#endif // FENETREPRINCIPALE_H
