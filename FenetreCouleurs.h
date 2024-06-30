#ifndef FENETRECOULEURS_H
#define FENETRECOULEURS_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QDataStream>
#include <QRadioButton>
#include <QTableWidget>
#include <QWidget>

#include "Degrade.h"
#include "Palette.h"

class Substrate;
class FancyLineEdit;

class FenetreCouleurs : public QWidget
{
    Q_OBJECT

    public:
        FenetreCouleurs(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
        void chargerSubstrate(Substrate *s) const;
        void enregistrer(QDataStream &out) const;
        void charger(QDataStream &in, const QString &version);
        QString getImageFond() const;
        bool getTailleAdapteeImage() const;

    private:
        QTableWidget *tableWidget;
        QList<WidgetPalette *> listeWidgetsPalette;
        QList<QPushButton *> listeBoutonsPalette;
        QList<QPushButton *> listeBoutonsGenererPalette;
        QList<WidgetDegrade *> listeWidgetsDegrade;
        QList<QPushButton *> listeBoutonsDegrade;
        QList<QPushButton *> listeBoutonsGenererDegrade;
        QList<FancyLineEdit *> listeFancyLineEdits;
        QList<QRadioButton *> listeRadioBoutonsPalette;
        QList<QRadioButton *> listeRadioBoutonsDegrade;
        QList<QRadioButton *> listeRadioBoutonsFichier;
        QRadioButton *radioBoutonPalettes;
        QRadioButton *radioBoutonDegrades;
        QRadioButton *radioBoutonFichiers;
        QPushButton *boutonGenererPalettes;
        QPushButton *boutonGenererDegrades;
        QPushButton *boutonParcourirFichierCommun;
        QButtonGroup *groupeBoutons;
        QCheckBox *checkBoxAlphaZonesFixee;
        QSpinBox *spinBoxAlphaZonesFixee;
        QCheckBox *checkBoxFondAvecImage;
        FancyLineEdit *fancyLineEditImageFond;
        QCheckBox *checkBoxTailleAdapteeImage;
        QList<int> listeMaxima;
        QList<QList<Couleur> > listePalettes;
        QList<Degrade> listeDegrades;
        QStringList listeFichiers;
        QList<bool> listeChoixPalette;
        QList<bool> listeChoixDegrade;
        bool alphaZonesFixee;
        int valeurAlphaZonesFixee;
        bool fondAvecImage;
        QString imageFond;
        bool tailleAdapteeImage;
        bool traitementEnCours;
        void enregistrerEtat();

    private slots:
        void ok();
        void annuler();
        void parcourirFichier();
        void modifierPalette();
        void genererPaletteAleatoire();
        void genererPalettesAleatoires();
        void modifierDegrade();
        void genererDegradeAleatoire();
        void genererDegradesAleatoires();
        void parcourirFichierCommun();
        void parcourirImageFond();
        void choixCouleursChange(QAbstractButton *bouton);
        void choixToutesCouleursChange(QAbstractButton *bouton);

    protected:
        void showEvent(QShowEvent *event);
        void closeEvent(QCloseEvent *event);
};

#endif // FENETRECOULEURS_H
