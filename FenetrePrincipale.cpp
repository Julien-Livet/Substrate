#include <QApplication>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QScreen>
#include <QStatusBar>
#include <QVBoxLayout>

#include "FenetreCouleurs.h"
#include "FenetreGenerateurs.h"
#include "FenetrePrincipale.h"
#include "Substrate.h"
#include "WidgetSubstrate.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent, Qt::WindowFlags f) : QMainWindow(parent, f)
{
    fenetreCouleurs = new FenetreCouleurs;
    fenetreCouleurs->hide();
    fenetreGenerateurs = new FenetreGenerateurs;
    fenetreGenerateurs->hide();

    substrateGenere = 0;
    compteurSubstrates = 0;
    choixSortie = -1;

    widgetSubstrate = 0;
    imageSubstrate = 0;
    printerSubstrate = new QPrinter;
    nomFichier = QString("Substrate.conf");

    spinBoxNombreCracksInitial = new QSpinBox(this);
    spinBoxNombreCracksInitial->setRange(1, QWIDGETSIZE_MAX);
    spinBoxNombreCracksInitial->setValue(100);
    spinBoxNombreCracksInitial->setKeyboardTracking(false);
    spinBoxNombreCracksFinal = new QSpinBox(this);
    spinBoxNombreCracksFinal->setRange(1, QWIDGETSIZE_MAX);
    spinBoxNombreCracksFinal->setValue(1000);
    spinBoxNombreCracksFinal->setKeyboardTracking(false);
    spinBoxNombreCracksFinal->setRange(spinBoxNombreCracksInitial->value(), QWIDGETSIZE_MAX);
    spinBoxNombreCracksInitial->setRange(1, spinBoxNombreCracksFinal->value());
    connect(spinBoxNombreCracksInitial, SIGNAL(valueChanged(int)), this, SLOT(nombreCracksInitialChange(int)));
    connect(spinBoxNombreCracksFinal, SIGNAL(valueChanged(int)), this, SLOT(nombreCracksFinalChange(int)));
    spinBoxNombreCracksSimultanesEnCreation = new QSpinBox(this);
    spinBoxNombreCracksSimultanesEnCreation->setRange(spinBoxNombreCracksInitial->value(),
                                                      spinBoxNombreCracksFinal->value());
    spinBoxNombreCracksSimultanesEnCreation->setValue(100);
    spinBoxProbabiliteDedoublementCrack = new QSpinBox(this);
    spinBoxProbabiliteDedoublementCrack->setRange(0, 100);
    spinBoxProbabiliteDedoublementCrack->setValue(25);
    spinBoxProbabiliteDedoublementCrack->setSuffix(tr("%"));

    checkBoxRemplissageZones = new QCheckBox(tr("Remplissage des zones"), this);
    spinBoxPourcentageCercles = new QSpinBox(this);
    spinBoxPourcentageCercles->setRange(0, 100);
    spinBoxPourcentageCercles->setValue(33);
    spinBoxPourcentageCercles->setSuffix(tr("%"));
    radioBoutonFilDeFer = new QRadioButton(tr("Fil de fer"), this);
    radioBoutonSable = new QRadioButton(tr("Sable"), this);
    radioBoutonSable->setChecked(true);
    spinBoxNombreGrains = new QSpinBox(this);
    spinBoxNombreGrains->setRange(1, QWIDGETSIZE_MAX);
    spinBoxNombreGrains->setValue(64);
    connect(radioBoutonSable, SIGNAL(toggled(bool)), spinBoxNombreGrains, SLOT(setEnabled(bool)));

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(radioBoutonFilDeFer);
    buttonGroup->addButton(radioBoutonSable);

    QPushButton *boutonGenerateurs = new QPushButton(tr("Générateurs"), this);
    connect(boutonGenerateurs, SIGNAL(clicked()), this, SLOT(montrerFenetreGenerateurs()));
    QPushButton *boutonCouleurs = new QPushButton(tr("Couleurs"), this);
    connect(boutonCouleurs, SIGNAL(clicked()), this, SLOT(montrerFenetreCouleurs()));
    QPushButton *boutonTailleCracks = new QPushButton(tr("Taille des cracks"), this);
    connect(boutonTailleCracks, SIGNAL(clicked()), this, SLOT(montrerFenetreInterpolation()));

    QGroupBox *groupBoxParametres = new QGroupBox(tr("Paramètres"), this);
    QGridLayout *gridLayoutParametres = new QGridLayout;
    gridLayoutParametres->addWidget(new QLabel(tr("Nombre de cracks initial :"), this), 0, 0);
    gridLayoutParametres->addWidget(spinBoxNombreCracksInitial, 0, 1);
    gridLayoutParametres->addWidget(new QLabel(tr("Nombre de cracks final :"), this), 0, 2);
    gridLayoutParametres->addWidget(spinBoxNombreCracksFinal, 0, 3);
    gridLayoutParametres->addWidget(new QLabel(tr("Nombre maximum de cracks\nsimultanés en création :"), this), 1, 0);
    gridLayoutParametres->addWidget(spinBoxNombreCracksSimultanesEnCreation, 1, 1);
    gridLayoutParametres->addWidget(new QLabel(tr("Probabilité de dédoublement d'un crack :"), this), 1, 2);
    gridLayoutParametres->addWidget(spinBoxProbabiliteDedoublementCrack, 1, 3);
    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    gridLayoutParametres->addWidget(frame, 2, 0, 1, 4);
    gridLayoutParametres->addWidget(new QLabel(tr("Pourcentage de cercles :"), this), 3, 0);
    gridLayoutParametres->addWidget(spinBoxPourcentageCercles, 3, 1);
    gridLayoutParametres->addWidget(checkBoxRemplissageZones, 3, 2);
    gridLayoutParametres->addWidget(radioBoutonFilDeFer, 4, 0);
    gridLayoutParametres->addWidget(radioBoutonSable, 4, 1);
    QLabel *label = new QLabel(tr("Nombre de grains :"), this);
    connect(radioBoutonSable, SIGNAL(toggled(bool)), label, SLOT(setEnabled(bool)));
    gridLayoutParametres->addWidget(label, 4, 2);
    gridLayoutParametres->addWidget(spinBoxNombreGrains, 4, 3);
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    gridLayoutParametres->addWidget(frame, 5, 0, 1, 4);
    gridLayoutParametres->addWidget(boutonGenerateurs, 6, 0);
    gridLayoutParametres->addWidget(boutonCouleurs, 6, 1);
    gridLayoutParametres->addWidget(boutonTailleCracks, 6, 2);
    groupBoxParametres->setLayout(gridLayoutParametres);

    spinBoxLongueur = new QSpinBox(this);
    spinBoxLongueur->setRange(8, QWIDGETSIZE_MAX);
    spinBoxLongueur->setValue(800);
    spinBoxHauteur = new QSpinBox(this);
    spinBoxHauteur->setRange(8, QWIDGETSIZE_MAX);
    spinBoxHauteur->setValue(600);
    spinBoxFacteurEchelle = new QDoubleSpinBox(this);
    spinBoxFacteurEchelle->setRange(1, 100);
    spinBoxFacteurEchelle->setDecimals(4);
    spinBoxFacteurEchelle->setValue(1);

    radioBoutonFenetre = new QRadioButton(tr("Fenêtre :"), this);
    radioBoutonFenetre->setChecked(true);
    checkBoxPleinEcran = new QCheckBox(tr("Plein écran"), this);
    checkBoxPleinEcran->setToolTip(tr("Taille adaptée au facteur d'échelle et à la taille de l'écran"));
    spinBoxPeriodeCroissance = new QSpinBox(this);
    spinBoxPeriodeCroissance->setRange(-1, 1000);
    spinBoxPeriodeCroissance->setValue(spinBoxPeriodeCroissance->minimum());
    spinBoxPeriodeCroissance->setSuffix(tr(" ms"));
    spinBoxPeriodeCroissance->setSpecialValueText(tr("Instantanée"));
    connect(radioBoutonFenetre, SIGNAL(toggled(bool)), checkBoxPleinEcran, SLOT(setEnabled(bool)));

    radioBoutonImage = new QRadioButton(tr("Image :"), this);
    fancyLineEditFichier = new FancyLineEdit(QString("Substrate.png"), this);
    fancyLineEditFichier->setDisabled(true);
    fancyLineEditFichier->setPixmap(QPixmap(QString(":/Images/Ouvrir.png")));
    fancyLineEditFichier->setAutoHideIcon(false);
    fancyLineEditFichier->iconButton()->setToolTip(tr("Parcourir"));
    fancyLineEditFichier->disconnect();
    connect(fancyLineEditFichier, SIGNAL(buttonClicked()), this, SLOT(parcourirFichierImage()));
    connect(radioBoutonImage, SIGNAL(toggled(bool)), fancyLineEditFichier, SLOT(setEnabled(bool)));
    checkBoxNumeroIntegre = new QCheckBox(tr("Numéro intégré :"), this);
    checkBoxNumeroIntegre->setDisabled(true);
    checkBoxNumeroIntegre->setChecked(true);
    connect(radioBoutonImage, SIGNAL(toggled(bool)), checkBoxNumeroIntegre, SLOT(setEnabled(bool)));
    spinBoxNumero = new QSpinBox(this);
    spinBoxNumero->setDisabled(true);
    spinBoxNumero->setRange(0, QWIDGETSIZE_MAX);
    spinBoxNumero->setValue(1);
    connect(radioBoutonImage, SIGNAL(toggled(bool)), spinBoxNumero, SLOT(setEnabled(bool)));
    checkBoxBouclage = new QCheckBox(tr("Bouclage"), this);
    checkBoxBouclage->setDisabled(true);
    checkBoxBouclage->setChecked(false);
    connect(radioBoutonImage, SIGNAL(toggled(bool)), checkBoxBouclage, SLOT(setEnabled(bool)));

    radioBoutonImprimante = new QRadioButton(tr("Imprimante :"), this);
    boutonConfigurerImprimante = new QPushButton(tr("Configurer"));
    boutonConfigurerImprimante->setDisabled(true);
    connect(boutonConfigurerImprimante, SIGNAL(clicked()), this, SLOT(configurerImprimante()));
    connect(radioBoutonImprimante, SIGNAL(toggled(bool)), boutonConfigurerImprimante, SLOT(setEnabled(bool)));
    comboBoxResolutionImprimante = new QComboBox(this);
    comboBoxResolutionImprimante->setDisabled(true);
    QStringList l;
    int r = qMin(QGuiApplication::screenAt(pos())->physicalDotsPerInchX(),
                 QGuiApplication::screenAt(pos())->physicalDotsPerInchY());
    int index = -1;
    for (int i = 1; i < 33; i *= 2)
    {
        l << tr("%1 DPI").arg(75 * i);
        if (75 * i < r && r < 75 * i * 2)
        {
            l << tr("%1 DPI").arg(r);
            index = l.size() - 1;
        }
        else if (r == 75 * i)
            index = l.size() - 1;
    }
    comboBoxResolutionImprimante->addItems(l);
    if (index != -1)
        comboBoxResolutionImprimante->setCurrentIndex(index);
    connect(radioBoutonImprimante, SIGNAL(toggled(bool)), comboBoxResolutionImprimante, SLOT(setEnabled(bool)));
    checkBoxAdaptationFormat = new QCheckBox(tr("Taille et facteur d'échelle adaptés au format et à la résolution"));
    checkBoxAdaptationFormat->setDisabled(true);
    checkBoxAdaptationFormat->setChecked(true);
    connect(radioBoutonImprimante, SIGNAL(toggled(bool)), checkBoxAdaptationFormat, SLOT(setEnabled(bool)));
    comboBoxPrioriteThread = new QComboBox(this);
    comboBoxPrioriteThread->addItems(QStringList() << tr("Basse") << tr("Inférieure à la normale")
                                                   << tr("Normale") << tr("Supérieure à la normale")
                                                   << tr("Haute") << tr("Temps critique")
                                                   << tr("Héritée du thread principal"));
    comboBoxPrioriteThread->setCurrentIndex(6);
    checkBoxAntialiasingActive = new QCheckBox(tr("Antialiasing activé"), this);
    checkBoxRepassageFinalCracks = new QCheckBox(tr("Repassage final des cracks"), this);
    checkBoxRepassageFinalCracks->setChecked(true);
    barreDeProgression = new QProgressBar(this);
    barreDeProgression->setTextVisible(true);
    boutonGenererAnnuler = new QPushButton(tr("Générer"), this);
    connect(boutonGenererAnnuler, SIGNAL(clicked()), this, SLOT(genererAnnuler()));
    connect(radioBoutonImprimante, SIGNAL(toggled(bool)), spinBoxPeriodeCroissance, SLOT(setDisabled(bool)));

    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(radioBoutonFenetre);
    buttonGroup->addButton(radioBoutonImage);
    buttonGroup->addButton(radioBoutonImprimante);

    QGroupBox *groupBoxGeneration = new QGroupBox(tr("Génération"), this);
    QGridLayout *gridLayoutGeneration = new QGridLayout;
    gridLayoutGeneration->addWidget(new QLabel(tr("Longueur :"), this), 0, 0);
    gridLayoutGeneration->addWidget(spinBoxLongueur, 0, 1);
    gridLayoutGeneration->addWidget(new QLabel(tr("Hauteur :"), this), 0, 2);
    gridLayoutGeneration->addWidget(spinBoxHauteur, 0, 3, 1, 2);
    gridLayoutGeneration->addWidget(new QLabel(tr("Facteur d'échelle :"), this), 1, 0);
    gridLayoutGeneration->addWidget(spinBoxFacteurEchelle, 1, 1);
    frame = new QFrame(this);
    gridLayoutGeneration->addWidget(frame, 2, 0, 1, 5);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    gridLayoutGeneration->addWidget(radioBoutonFenetre, 3, 0);
    gridLayoutGeneration->addWidget(checkBoxPleinEcran, 3, 1);
    label = new QLabel(tr("Période de croissance :"), this);
    connect(radioBoutonFenetre, SIGNAL(toggled(bool)), label, SLOT(setEnabled(bool)));
    gridLayoutGeneration->addWidget(label, 3, 2);
    gridLayoutGeneration->addWidget(spinBoxPeriodeCroissance, 3, 3, 1, 2);
    gridLayoutGeneration->addWidget(radioBoutonImage, 4, 0);
    gridLayoutGeneration->addWidget(fancyLineEditFichier, 4, 1);
    gridLayoutGeneration->addWidget(checkBoxNumeroIntegre, 4, 2);
    gridLayoutGeneration->addWidget(spinBoxNumero, 4, 3);
    gridLayoutGeneration->addWidget(checkBoxBouclage, 4, 4);
    gridLayoutGeneration->addWidget(radioBoutonImprimante, 5, 0);
    gridLayoutGeneration->addWidget(boutonConfigurerImprimante, 5, 1);
    label = new QLabel(tr("Résolution :"), this);
    label->setDisabled(true);
    connect(radioBoutonImprimante, SIGNAL(toggled(bool)), label, SLOT(setEnabled(bool)));
    gridLayoutGeneration->addWidget(label, 5, 2);
    gridLayoutGeneration->addWidget(comboBoxResolutionImprimante, 5, 3, 1, 2);
    gridLayoutGeneration->addWidget(checkBoxAdaptationFormat, 6, 1, 1, 4);
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    gridLayoutGeneration->addWidget(frame, 7, 0, 1, 5);
    gridLayoutGeneration->addWidget(new QLabel(tr("Priorité du thread :"), this), 8, 0);
    gridLayoutGeneration->addWidget(comboBoxPrioriteThread, 8, 1);
    gridLayoutGeneration->addWidget(checkBoxAntialiasingActive, 8, 2);
    gridLayoutGeneration->addWidget(checkBoxRepassageFinalCracks, 8, 3, 1, 2);
    gridLayoutGeneration->addWidget(barreDeProgression, 9, 0, 1, 4);
    gridLayoutGeneration->addWidget(boutonGenererAnnuler, 9, 4);
    groupBoxGeneration->setLayout(gridLayoutGeneration);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(groupBoxParametres);
    vBoxLayout->addWidget(groupBoxGeneration);
    QWidget *w = new QWidget(this);
    w->setLayout(vBoxLayout);
    setCentralWidget(w);

    QMenu *menuFichier = menuBar()->addMenu(tr("&Fichier"));
    QAction *actionOuvrir = menuFichier->addAction(QIcon::fromTheme(QString("document-open"),
                                                                    QIcon(":/Images/Ouvrir.png")), tr("&Ouvrir"));
    actionOuvrir->setShortcut(QKeySequence::Open);
    menuFichier->addSeparator();
    QAction *actionEnregistrer = menuFichier->addAction(QIcon::fromTheme(QString("document-save"),
                                                                         QIcon(":/Images/Enregistrer.png")),
                                                        tr("&Enregistrer"));
    actionEnregistrer->setShortcut(QKeySequence::Save);
    QAction *actionEnregistrerSous = menuFichier->addAction(QIcon::fromTheme(QString("document-save-as"),
                                                                             QIcon(":/Images/Enregistrer_sous.png")),
                                                            tr("En&registrer sous..."));
    actionEnregistrerSous->setShortcut(QKeySequence::SaveAs);
    menuFichier->addSeparator();
    QAction *actionQuitter = menuFichier->addAction(QIcon::fromTheme(QString("system-shutdown"),
                                                                     QIcon(":/Images/Quitter.png")),
                                                    tr("&Quitter"));
    actionQuitter->setShortcut(QString("Ctrl+Q"));

    QMenu *menuAPropos = menuBar()->addMenu(tr("&À propos"));
    QAction *actionAPropos = menuAPropos->addAction(QIcon::fromTheme(QString("help-about"), QIcon(":/Images/A_propos.png")), tr("À propos de &Substrate..."));
    QAction *actionAProposQt = menuAPropos->addAction(QIcon(":/Images/A_propos_de_Qt.png"), tr("À propos de Q&t..."));

    connect(actionOuvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));
    connect(actionEnregistrer, SIGNAL(triggered()), this, SLOT(enregistrer()));
    connect(actionEnregistrerSous, SIGNAL(triggered()), this, SLOT(enregistrerSous()));
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(quitter()));
    connect(actionAPropos, SIGNAL(triggered()), this, SLOT(aPropos()));
    connect(actionAProposQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    setWindowTitle(tr("Substrate"));
    setWindowIcon(QIcon(":/Images/Substrate.png"));

    chargerFichier("Substrate.conf");
    nomFichier.clear();

    show();

    QPoint p{QGuiApplication::screenAt(pos())->geometry().center() - rect().center()};
    p.ry() -= 40;
    move(p);

    statusBar()->showMessage(tr("Bienvenue dans Substrate !"), 5000);

    if (qApp->arguments().contains("-g") || qApp->arguments().contains("--generate"))
        boutonGenererAnnuler->click();

    if (qApp->arguments().contains("-m") || qApp->arguments().contains("--minimized"))
        showMinimized();

    if (qApp->arguments().contains("-h") || qApp->arguments().contains("--hidden"))
        hide();
}

FenetrePrincipale::~FenetrePrincipale()
{
    delete fenetreCouleurs;
    delete fenetreGenerateurs;
}

void FenetrePrincipale::enregistrerFichier(QString fichier)
{
    if (fichier.isEmpty())
    {
        fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer la configuration"), nomFichier,
                                               tr("Configuration (*.conf)"));

        if (fichier.isEmpty())
            return;
    }

    nomFichier = fichier;

    QFile f(nomFichier);

    if (f.open(QFile::WriteOnly))
    {
        QDataStream out(&f);
        enregistrer(out);
        f.close();
    }
}

void FenetrePrincipale::chargerFichier(QString fichier)
{
    if (fichier.isEmpty())
    {
        fichier = QFileDialog::getOpenFileName(this, tr("Charger une configuration"), nomFichier,
                                               tr("Configuration (*.conf)"));

        if (fichier.isEmpty())
            return;
    }

    nomFichier = fichier;

    QFile f(nomFichier);

    if (f.open(QFile::ReadOnly))
    {
        QDataStream in(&f);
        charger(in);
        f.close();
    }
}

void FenetrePrincipale::chargerSubstrate(Substrate *s) const
{
    statusBar()->showMessage(tr("Chargement des paramètres du substrate en cours..."));
    s->setNombreCracksInitial(spinBoxNombreCracksInitial->value());
    s->setNombreCracksFinal(spinBoxNombreCracksFinal->value());
    s->setNombreCracksSimultanesEnCreation(spinBoxNombreCracksSimultanesEnCreation->value());
    s->setProbabiliteDeboublementCrack(spinBoxProbabiliteDedoublementCrack->value());
    s->setCirclePercent(spinBoxPourcentageCercles->value());
    s->setRemplissageZones(checkBoxRemplissageZones->isChecked());
    s->setGrains(spinBoxNombreGrains->value());
    s->setFilDeFer(radioBoutonFilDeFer->isChecked());
    fenetreGenerateurs->chargerSubstrate(s);
    fenetreCouleurs->chargerSubstrate(s);
    s->setInterpolation(interpolation);
    s->setRect(QRect(0, 0, longueur, hauteur));
    s->setFacteurEchelle(facteurEchelle);
    s->setPrioriteThread(QThread::Priority(comboBoxPrioriteThread->currentIndex()+1));
    s->setAntialiasingActive(checkBoxAntialiasingActive->isChecked());
    s->setRepassageFinalCracks(checkBoxRepassageFinalCracks->isChecked());
}

void FenetrePrincipale::enregistrer(QDataStream &out) const
{
    out << out.version();
    out << VERSION;
    out << spinBoxNombreCracksInitial->value();
    out << spinBoxNombreCracksFinal->value();
    out << spinBoxNombreCracksSimultanesEnCreation->value();
    out << spinBoxProbabiliteDedoublementCrack->value();
    out << spinBoxPourcentageCercles->value();
    out << checkBoxRemplissageZones->isChecked();
    out << spinBoxNombreGrains->value();
    out << radioBoutonFilDeFer->isChecked();
    fenetreGenerateurs->enregistrer(out);
    fenetreCouleurs->enregistrer(out);
    interpolation.enregistrer(out);
    out << spinBoxLongueur->value();
    out << spinBoxHauteur->value();
    out << spinBoxFacteurEchelle->value();
    out << radioBoutonFenetre->isChecked();
    out << checkBoxPleinEcran->isChecked();
    out << spinBoxPeriodeCroissance->value();
    out << radioBoutonImage->isChecked();
    out << fancyLineEditFichier->text();
    out << checkBoxNumeroIntegre->isChecked();
    out << spinBoxNumero->value();
    out << comboBoxResolutionImprimante->currentIndex();
    out << checkBoxAdaptationFormat->isChecked();
    out << comboBoxPrioriteThread->currentIndex();
    out << checkBoxAntialiasingActive->isChecked();
    out << checkBoxRepassageFinalCracks->isChecked();
}

void FenetrePrincipale::charger(QDataStream &in)
{
    int entier;
    in >> entier;
    in.setVersion(entier);
    QString version;
    in >> version;
    in >> entier;
    spinBoxNombreCracksInitial->setValue(entier);
    in >> entier;
    spinBoxNombreCracksFinal->setValue(entier);
    in >> entier;
    spinBoxNombreCracksSimultanesEnCreation->setValue(entier);
    in >> entier;
    spinBoxProbabiliteDedoublementCrack->setValue(entier);
    in >> entier;
    spinBoxPourcentageCercles->setValue(entier);
    bool b;
    in >> b;
    checkBoxRemplissageZones->setChecked(b);
    in >> entier;
    spinBoxNombreGrains->setValue(entier);
    in >> b;
    radioBoutonFilDeFer->setChecked(b);
    fenetreGenerateurs->charger(in, version);
    fenetreCouleurs->charger(in, version);
    interpolation.charger(in, version);
    in >> entier;
    spinBoxLongueur->setValue(entier);
    in >> entier;
    spinBoxHauteur->setValue(entier);
    qreal r;
    in >> r;
    spinBoxFacteurEchelle->setValue(r);
    bool b1;
    in >> b1;
    radioBoutonFenetre->setChecked(b1);
    in >> b;
    checkBoxPleinEcran->setChecked(b);
    in >> entier;
    spinBoxPeriodeCroissance->setValue(entier);
    bool b2;
    in >> b2;
    radioBoutonImage->setChecked(b2);
    QString s;
    in >> s;
    fancyLineEditFichier->setText(s);
    in >> b;
    checkBoxNumeroIntegre->setChecked(b);
    in >> entier;
    spinBoxNumero->setValue(entier);
    if (!b1 && !b2)
        radioBoutonImprimante->setChecked(true);
    in >> entier;
    comboBoxResolutionImprimante->setCurrentIndex(entier);
    in >> b;
    checkBoxAdaptationFormat->setChecked(b);
    in >> entier;
    comboBoxPrioriteThread->setCurrentIndex(entier);
    in >> b;
    checkBoxAntialiasingActive->setChecked(b);
    in >> b;
    checkBoxRepassageFinalCracks->setChecked(b);
}

void FenetrePrincipale::montrerFenetreCouleurs()
{
    fenetreCouleurs->show();
    fenetreCouleurs->raise();
}

void FenetrePrincipale::montrerFenetreGenerateurs()
{
    fenetreGenerateurs->show();
    fenetreGenerateurs->raise();
}

void FenetrePrincipale::montrerFenetreInterpolation()
{
    interpolation.exec(this);
}

void FenetrePrincipale::genererAnnuler()
{
    if (boutonGenererAnnuler->text() == tr("Générer"))
    {
        barreDeProgression->setValue(0);
        substrateGenere = new Substrate;//(this);
        connect(substrateGenere, SIGNAL(chargementChange(int)), barreDeProgression, SLOT(setValue(int)));
        connect(substrateGenere, SIGNAL(termine()), this, SLOT(substrateTermine()));
        connect(substrateGenere, SIGNAL(arrete()), substrateGenere, SLOT(deleteLater()));
        connect(substrateGenere, SIGNAL(debutRemplissageZones()), this, SLOT(afficherMessageRemplissage()));

        bool generationOk = true;
        QPaintDevice *device = 0;
        int periode = -1;
        longueur = spinBoxLongueur->value();
        hauteur = spinBoxHauteur->value();
        facteurEchelle = spinBoxFacteurEchelle->value();
        compteurSubstrates++;

        if (fenetreCouleurs->getTailleAdapteeImage())
        {
            QImage image(fenetreCouleurs->getImageFond());
            if (!image.isNull())
            {
                longueur = (double)image.width()/facteurEchelle;
                hauteur = (double)image.height()/facteurEchelle;
                image = QImage();
            }
        }

        if (radioBoutonFenetre->isChecked())
        {
            choixSortie = 0;
            if (checkBoxPleinEcran->isChecked())
            {
                longueur = (double)QGuiApplication::screenAt(pos())->size().width() / facteurEchelle;
                hauteur = (double)QGuiApplication::screenAt(pos())->size().height() / facteurEchelle;
            }
            chargerSubstrate(substrateGenere);
            widgetSubstrate = new WidgetSubstrate(substrateGenere);
            device = widgetSubstrate->getPixmap();
            if (widgetSubstrate->getPixmap()->isNull())
                generationOk = false;
            periode = spinBoxPeriodeCroissance->value();
        }
        else if (radioBoutonImage->isChecked())
        {
            choixSortie = 1;
            periode = spinBoxPeriodeCroissance->value();
            if (fancyLineEditFichier->text().isEmpty())
            {
                generationOk = false;
                QMessageBox::warning(this, tr("Destination incorrecte"),
                                     tr("Veuillez choisir un fichier de destination !"),
                                     QMessageBox::Ok);
            }
            else
            {
                int l = longueur, h = hauteur;
                if (!fenetreCouleurs->getTailleAdapteeImage())
                {
                    l = longueur*facteurEchelle;
                    h = hauteur*facteurEchelle;
                }
                imageSubstrate = new QImage(QSize(l, h), QImage::Format_ARGB32);
                if (!imageSubstrate->isNull())
                {
                    imageSubstrate->fill(QColor(0, 0, 0, 0));
                    chargerSubstrate(substrateGenere);
                    device = imageSubstrate;
                }
                else
                {
                    generationOk = false;
                    QMessageBox::warning(this, tr("Image trop grande"),
                                         tr("La taille de l'image est beaucoup trop grande pour être créée."),
                                         QMessageBox::Ok);
                }
            }
        }
        else// if (radioBoutonImprimante->isChecked())
        {
            choixSortie = 2;
            if (printerSubstrate->outputFormat() == QPrinter::PdfFormat)
                printerSubstrate->setOutputFileName(tr("Substrate n°") + QString::number(compteurSubstrates)
                                                    + QString(".pdf"));

            QPrintDialog printDialog(printerSubstrate, this);

            if (printDialog.exec() == QPrintDialog::Accepted)
            {
                if (!printerSubstrate->outputFileName().isEmpty())
                    printerSubstrate->setDocName(QFileInfo(printerSubstrate->outputFileName()).baseName());
                else
                    printerSubstrate->setDocName(tr("Substrate n°") + QString::number(compteurSubstrates));
                printerSubstrate->setResolution(comboBoxResolutionImprimante->currentText().remove(QString(" DPI")).toInt());
                if (checkBoxAdaptationFormat->isChecked())
                {
                    facteurEchelle = (double)printerSubstrate->resolution() / fmin(QGuiApplication::screenAt(pos())->physicalDotsPerInchX(),
                                                                                   QGuiApplication::screenAt(pos())->physicalDotsPerInchY());
                    if (facteurEchelle < 1)
                        facteurEchelle = 1;
                    longueur = (double)printerSubstrate->pageRect(QPrinter::Millimeter).width() / facteurEchelle;
                    hauteur = (double)printerSubstrate->pageRect(QPrinter::Millimeter).height() / facteurEchelle;
                }
                imageSubstrate = new QImage(QSize(longueur, hauteur) * facteurEchelle, QImage::Format_ARGB32);
                if (!imageSubstrate->isNull())
                {
                    chargerSubstrate(substrateGenere);
                    device = imageSubstrate;
                }
                else
                {
                    generationOk = false;
                    QMessageBox::warning(this, tr("Image trop grande"),
                                         tr("La taille de l'image est beaucoup trop grande pour être créée."),
                                         QMessageBox::Ok);
                }
            }
            else
                generationOk = false;
        }

        if (generationOk)
        {
            boutonGenererAnnuler->setText(tr("Annuler"));
            Substrate *s = 0;
            if (widgetSubstrate || imageSubstrate)
            {
                disconnect(substrateGenere, SIGNAL(arrete()), substrateGenere, SLOT(deleteLater()));
                if (periode >= 0)
                {
                    if (widgetSubstrate)
                    {
                        if (checkBoxPleinEcran->isChecked())
                            widgetSubstrate->showFullScreen();
                        else
                        {
                            widgetSubstrate->show();
                            auto p{QGuiApplication::screenAt(pos())->geometry().center() - widgetSubstrate->rect().center()};
                            p.ry() -= 40;
                            widgetSubstrate->move(p);
                        }
                        disconnect(substrateGenere, SIGNAL(chargementChange(int)), barreDeProgression, SLOT(setValue(int)));
                        disconnect(substrateGenere, SIGNAL(termine()), this, SLOT(substrateTermine()));
                        disconnect(substrateGenere, SIGNAL(debutRemplissageZones()), this, SLOT(afficherMessageRemplissage()));
                        connect(substrateGenere, SIGNAL(modifie()), widgetSubstrate, SLOT(update()));
                    }
                    else if (imageSubstrate)
                    {
                        connect(substrateGenere, SIGNAL(modifie()), this, SLOT(rafraichirImage()));
                        connect(substrateGenere, SIGNAL(arrete()), this, SLOT(finGenerationImage()));
                        connect(substrateGenere, SIGNAL(termine()), this, SLOT(finGenerationImage()));
                    }
                    else
                    {
                        boutonGenererAnnuler->setText(tr("Générer"));
                        widgetSubstrate->setWindowTitle(tr("Substrate n°")+QString::number(compteurSubstrates));
                        widgetSubstrate = 0;
                        s = substrateGenere;
                    }
                }
            }
            statusBar()->showMessage(tr("Génération du substrate en cours... (%1x%2 : %3)").arg(longueur)
                                                                                           .arg(hauteur)
                                                                                           .arg(facteurEchelle));
            substrateGenere->generer(device, QPointF(), compteurSubstrates, periode);
            if (s)
            {
                statusBar()->clearMessage();
                fenetreGenerateurs->chargerClesSubstrate(s);
                substrateGenere = 0;
            }
        }
        else
        {
            delete substrateGenere;
            substrateGenere = 0;
            if (widgetSubstrate)
            {
                delete widgetSubstrate;
                widgetSubstrate = 0;
            }
            if (imageSubstrate)
            {
                delete imageSubstrate;
                imageSubstrate = 0;
            }
            compteurSubstrates--;
            choixSortie = -1;
            statusBar()->showMessage(tr("Génération du substrate annulée !"), 5000);
        }
    }
    else
    {
        if (substrateGenere)
        {
            delete substrateGenere;
            substrateGenere = 0;
        }
        if (choixSortie == 2)//Imprimante
        {
            if (imageSubstrate)
            {
                delete imageSubstrate;
                imageSubstrate = 0;
            }
        }
        choixSortie = -1;
        boutonGenererAnnuler->setText(tr("Générer"));
        barreDeProgression->setValue(0);
        statusBar()->showMessage(tr("Génération du substrate annulée !"), 5000);
    }
}

void FenetrePrincipale::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);

    event->setAccepted(quitter());
}

bool FenetrePrincipale::quitter()
{
    if (QMessageBox::warning(this, tr("Enregistrer la configuration"),
                             tr("Voulez-vous enregistrer la configuration actuelle pour le prochain démarrage ?"),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        enregistrerFichier(QString("Substrate.conf"));

    if (boutonGenererAnnuler->text() == tr("Annuler"))
        genererAnnuler();

    qApp->quit();

    return true;
}

void FenetrePrincipale::parcourirFichierImage()
{
    QString texte = QFileDialog::getSaveFileName(this, tr("Choisir un fichier de destination"),
                                                 fancyLineEditFichier->text(),
                                                 tr("Images (*.bmp *.jpeg *.jpg *.png *.tiff)"));

    if (!texte.isNull())
        fancyLineEditFichier->setText(texte);
}

void FenetrePrincipale::substrateTermine()
{
    Substrate *substrate = qobject_cast<Substrate *>(sender());

    if (!choixSortie)//Fenêtre
    {
        if (substrate)
        {
            if (!substrate->getGenerationCadencee() && widgetSubstrate)
            {
                if (checkBoxPleinEcran->isChecked())
                    widgetSubstrate->showFullScreen();
                else
                {
                    widgetSubstrate->show();
                    auto p{QGuiApplication::screenAt(pos())->geometry().center() - widgetSubstrate->rect().center()};
                    p.ry() -= 40;
                    widgetSubstrate->move(p);
                    widgetSubstrate->setWindowTitle(tr("Substrate n°%1").arg(substrate->getNumeroSubstrate()));
                }
            }
        }
    }
    else if (choixSortie == 1)//Image
    {
        auto s{fancyLineEditFichier->text()};
        if (checkBoxNumeroIntegre->isChecked())
        {
            QFileInfo f(s);
            s = tr("%1/%2 n°%3.%4").arg(f.absolutePath()).arg(f.baseName())
                                   .arg(spinBoxNumero->value()).arg(f.completeSuffix());
            spinBoxNumero->setValue(spinBoxNumero->value()+1);
        }
        imageSubstrate->save(s, 0, 100);
        delete imageSubstrate;
        imageSubstrate = 0;
    }
    else// if (choixSortie == 2)//Imprimante
    {
        QPainter painter(printerSubstrate);
        painter.drawImage(0, 0, *imageSubstrate);
        painter.end();
        delete imageSubstrate;
        imageSubstrate = 0;
    }

    if (substrateGenere)
    {
        fenetreGenerateurs->chargerClesSubstrate(substrateGenere);
        if (choixSortie)
            substrateGenere->deleteLater();
        substrateGenere = 0;
    }
    barreDeProgression->setValue(0);
    boutonGenererAnnuler->setText(tr("Générer"));
    statusBar()->showMessage(tr("Génération du substrate terminée !"), 5000);
}

void FenetrePrincipale::configurerImprimante()
{
    QPageSetupDialog pageSetup(printerSubstrate, this);

    pageSetup.exec();
}

void FenetrePrincipale::nombreCracksInitialChange(int v)
{
    spinBoxNombreCracksFinal->setRange(v, QWIDGETSIZE_MAX);
    spinBoxNombreCracksSimultanesEnCreation->setRange(v, spinBoxNombreCracksFinal->value());
    spinBoxNombreCracksInitial->setRange(1, spinBoxNombreCracksFinal->value());
}

void FenetrePrincipale::nombreCracksFinalChange(int v)
{
    spinBoxNombreCracksInitial->setRange(1, v);
    spinBoxNombreCracksSimultanesEnCreation->setRange(spinBoxNombreCracksInitial->value(), v);
    spinBoxNombreCracksFinal->setRange(spinBoxNombreCracksInitial->value(), QWIDGETSIZE_MAX);
}

void FenetrePrincipale::enregistrer()
{
    enregistrerFichier(nomFichier);
}

void FenetrePrincipale::enregistrerSous()
{
    enregistrerFichier(QString());
}

void FenetrePrincipale::ouvrir()
{
    chargerFichier(QString());
}

void FenetrePrincipale::aPropos()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("À propos de Substrate"));

    QDialogButtonBox dialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    connect(dialogButtonBox.button(QDialogButtonBox::Ok), SIGNAL(clicked()), &dialog, SLOT(accept()));

    QLabel labelIcone(&dialog);
    labelIcone.setPixmap(QPixmap(QString(":/Images/Substrate.png")));
    labelIcone.setContentsMargins(10, 10, 10, 10);
    QLabel labelTexte(tr("<h3>Substrate %1</h3>"
                         "<p>Substrate est un logiciel gratuit et libre de distribution réalisé en C++ avec le framework Qt %2"
                         " par Julien LIVET en 2013. Ce logiciel est destiné à une utilisation libre et ne peut être vendu.</p>"
                         "<p>Cette application se base sur l'algorithme Substrate développé par J. Tarbell en juin 2004 à Albuquerque "
                         "depuis le site <a href=\"http://complexification.net\">complexification.net</a> .</p>"
                         "<p>Pour toute information sur le logiciel, vous pouvez me contacter par mail à l'adresse suivante : "
                         "<a href=\"mailto:julien.livet@free.fr\">julien.livet@free.fr</a> .</p>").arg(VERSION).arg(QT_VERSION_STR), &dialog);
    labelTexte.setWordWrap(true);
    labelTexte.setAlignment(Qt::AlignJustify);
    labelTexte.setOpenExternalLinks(true);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(&labelIcone, 0, Qt::AlignHCenter | Qt::AlignTop);
    hBoxLayout->addWidget(&labelTexte, 0, Qt::AlignHCenter | Qt::AlignTop);
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addWidget(&dialogButtonBox);
    dialog.setLayout(vBoxLayout);

    dialog.exec();

    hBoxLayout->deleteLater();
    vBoxLayout->deleteLater();
}

void FenetrePrincipale::afficherMessageRemplissage()
{
    statusBar()->showMessage(tr("Remplissage des zones du substrate en cours..."));
}

void FenetrePrincipale::rafraichirImage()
{
    imageSubstrate->save(fancyLineEditFichier->text(), 0, 100);
}

void FenetrePrincipale::finGenerationImage()
{
    if (checkBoxBouclage->isChecked())
        boutonGenererAnnuler->click();
}
