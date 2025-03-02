#include <QApplication>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFrame>
#include <QHeaderView>
#include <QPushButton>
#include <QScreen>
#include <QSpinBox>
#include <QVBoxLayout>

#include "constantes.h"
#include "FancyLineEdit.h"
#include "FenetreCouleurs.h"
#include "Substrate.h"

FenetreCouleurs::FenetreCouleurs(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    traitementEnCours = false;

    tableWidget = new QTableWidget(NOMBRE_COULEURS, 5, this);
    tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Couleur") << tr("Maximum") << tr("Palette") << tr("Dégradé") << tr("Fichier"));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    tableWidget->horizontalHeader()->setResizeMode(4, QHeaderView::Stretch);
    tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

    radioBoutonPalettes = new QRadioButton(tr("Palettes :"), this);
    radioBoutonPalettes->setChecked(true);
    radioBoutonDegrades = new QRadioButton(tr("Dégradés :"), this);
    radioBoutonFichiers = new QRadioButton(tr("Fichiers :"), this);
    groupeBoutons = new QButtonGroup(this);
    groupeBoutons->addButton(radioBoutonPalettes);
    groupeBoutons->addButton(radioBoutonDegrades);
    groupeBoutons->addButton(radioBoutonFichiers);
    connect(groupeBoutons, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(choixToutesCouleursChange(QAbstractButton *)));

    QList<int> l;

    for (int i = 0; i < NOMBRE_COULEURS; i++)
    {
        //Palette
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tableWidget->setItem(i, 0, item);
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setRange(0, QWIDGETSIZE_MAX);
        spinBox->setSpecialValueText(tr("Toutes les couleurs disponibles"));
        spinBox->setValue(100);
        spinBox->setFixedWidth(50);
        tableWidget->setCellWidget(i, 1, spinBox);
        QRadioButton *radioBoutonPalette = new QRadioButton(this);
        radioBoutonPalette->setChecked(true);
        listeRadioBoutonsPalette << radioBoutonPalette;
        WidgetPalette *widgetPalette = new WidgetPalette(this);
        widgetPalette->setMinimumWidth(128);
        listeWidgetsPalette << widgetPalette;
        QPushButton *boutonGenererPalette = new QPushButton(this);
        boutonGenererPalette->setIcon(QPixmap(":/Images/Engrenage.png"));
        boutonGenererPalette->setIconSize(QSize(16, 16));
        boutonGenererPalette->setFlat(true);
        boutonGenererPalette->setToolTip(tr("Générer une palette aléatoire"));
        listeBoutonsGenererPalette << boutonGenererPalette;
        QPushButton *boutonPalette = new QPushButton(this);
        boutonPalette->setIcon(QPixmap(":/Images/Palette.png"));
        boutonPalette->setIconSize(QSize(16, 16));
        boutonPalette->setFlat(true);
        boutonPalette->setToolTip(tr("Modifier la palette"));
        listeBoutonsPalette << boutonPalette;
        QWidget *w = new QWidget(this);
        QHBoxLayout *hBoxLayout = new QHBoxLayout;
        hBoxLayout->setContentsMargins(0, 0, 0, 0);
        hBoxLayout->addWidget(radioBoutonPalette);
        hBoxLayout->addWidget(widgetPalette);
        hBoxLayout->addWidget(boutonGenererPalette);
        hBoxLayout->addWidget(boutonPalette);
        w->setLayout(hBoxLayout);
        tableWidget->setCellWidget(i, 2, w);
        connect(radioBoutonPalette, SIGNAL(toggled(bool)), widgetPalette, SLOT(setEnabled(bool)));
        connect(radioBoutonPalette, SIGNAL(toggled(bool)), boutonGenererPalette, SLOT(setEnabled(bool)));
        connect(radioBoutonPalette, SIGNAL(toggled(bool)), boutonPalette, SLOT(setEnabled(bool)));
        connect(boutonPalette, SIGNAL(clicked()), this, SLOT(modifierPalette()));
        connect(boutonGenererPalette, SIGNAL(clicked()), this, SLOT(genererPaletteAleatoire()));
        //Dégradé
        QRadioButton *radioBoutonDegrade = new QRadioButton(this);
        listeRadioBoutonsDegrade << radioBoutonDegrade;
        WidgetDegrade *widgetDegrade = new WidgetDegrade(this);
        widgetDegrade->setDisabled(true);
        widgetDegrade->setMinimumWidth(128);
        listeWidgetsDegrade << widgetDegrade;
        QPushButton *boutonGenererDegrade = new QPushButton(this);
        boutonGenererDegrade->setDisabled(true);
        boutonGenererDegrade->setIcon(QPixmap(":/Images/Engrenage.png"));
        boutonGenererDegrade->setIconSize(QSize(16, 16));
        boutonGenererDegrade->setFlat(true);
        boutonGenererDegrade->setToolTip(tr("Générer un dégradé aléatoire"));
        listeBoutonsGenererDegrade << boutonGenererDegrade;
        QPushButton *boutonDegrade = new QPushButton(this);
        boutonDegrade->setDisabled(true);
        boutonDegrade->setIcon(QPixmap(":/Images/Palette.png"));
        boutonDegrade->setIconSize(QSize(16, 16));
        boutonDegrade->setFlat(true);
        boutonDegrade->setToolTip(tr("Modifier le dégradé"));
        listeBoutonsDegrade << boutonDegrade;
        w = new QWidget(this);
        hBoxLayout = new QHBoxLayout;
        hBoxLayout->setContentsMargins(0, 0, 0, 0);
        hBoxLayout->addWidget(radioBoutonDegrade);
        hBoxLayout->addWidget(widgetDegrade);
        hBoxLayout->addWidget(boutonGenererDegrade);
        hBoxLayout->addWidget(boutonDegrade);
        w->setLayout(hBoxLayout);
        tableWidget->setCellWidget(i, 3, w);
        connect(radioBoutonDegrade, SIGNAL(toggled(bool)), widgetDegrade, SLOT(setEnabled(bool)));
        connect(radioBoutonDegrade, SIGNAL(toggled(bool)), boutonGenererDegrade, SLOT(setEnabled(bool)));
        connect(radioBoutonDegrade, SIGNAL(toggled(bool)), boutonDegrade, SLOT(setEnabled(bool)));
        connect(boutonDegrade, SIGNAL(clicked()), this, SLOT(modifierDegrade()));
        connect(boutonGenererDegrade, SIGNAL(clicked()), this, SLOT(genererDegradeAleatoire()));
        //Fichier
        QRadioButton *radioBoutonFichier = new QRadioButton(this);
        listeRadioBoutonsFichier << radioBoutonFichier;
        FancyLineEdit *fancyLineEdit = new FancyLineEdit(QString("pollockShimmering.gif"), this);
        fancyLineEdit->setPixmap(QPixmap(QString(":/Images/Ouvrir.png")));
        fancyLineEdit->setAutoHideIcon(false);
        fancyLineEdit->setReadOnly(true);
        fancyLineEdit->iconButton()->setToolTip(tr("Parcourir"));
        fancyLineEdit->disconnect();
        connect(fancyLineEdit, SIGNAL(buttonClicked()), this, SLOT(parcourirFichier()));
        fancyLineEdit->setDisabled(true);
        listeFancyLineEdits << fancyLineEdit;
        l << fancyLineEdit->minimumWidth();
        fancyLineEdit->setMinimumWidth(256);
        connect(radioBoutonFichier, SIGNAL(toggled(bool)), fancyLineEdit, SLOT(setEnabled(bool)));
        w = new QWidget(this);
        hBoxLayout = new QHBoxLayout;
        hBoxLayout->setContentsMargins(0, 0, 0, 0);
        hBoxLayout->addWidget(radioBoutonFichier);
        hBoxLayout->addWidget(fancyLineEdit);
        w->setLayout(hBoxLayout);
        tableWidget->setCellWidget(i, 4, w);
        QButtonGroup *buttonGroup = new QButtonGroup(this);
        buttonGroup->addButton(radioBoutonPalette);
        buttonGroup->addButton(radioBoutonDegrade);
        buttonGroup->addButton(radioBoutonFichier);
        connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(choixCouleursChange(QAbstractButton *)));
    }

    tableWidget->item(0, 0)->setText(tr("Cracks"));
    tableWidget->item(1, 0)->setText(tr("Sable"));
    tableWidget->item(2, 0)->setText(tr("Zones"));
    tableWidget->item(3, 0)->setText(tr("Fond"));

    Couleur c;
    c.nombre = 1;
    c.couleur = Qt::black;
    listeWidgetsPalette[0]->setListeCouleurs(QList<Couleur>() << c);
    Degrade degrade;
    degrade.couleurDepart = Qt::blue;
    degrade.listeCouleurs << QPair<int, QColor>(128, Qt::red);
    listeWidgetsDegrade[1]->setDegrade(degrade);
    degrade.couleurDepart = Qt::yellow;
    degrade.listeCouleurs.first().second = Qt::darkMagenta;
    listeWidgetsDegrade[2]->setDegrade(degrade);
    c.couleur = Qt::white;
    listeWidgetsPalette[3]->setListeCouleurs(QList<Couleur>() << c);

    tableWidget->resizeColumnsToContents();
    tableWidget->resizeRowsToContents();

    for (int i = 0; i < NOMBRE_COULEURS; i++)
    {
        QSpinBox *spinBox = qobject_cast<QSpinBox *>(tableWidget->cellWidget(i, 1));
        spinBox->setMinimumWidth(0);
        spinBox->setMaximumWidth(QWIDGETSIZE_MAX);
        listeWidgetsPalette[i]->setMinimumWidth(32);
        listeWidgetsDegrade[i]->setMinimumWidth(32);
        listeFancyLineEdits[i]->setMinimumWidth(l[i]);
    }

    boutonGenererPalettes = new QPushButton(tr("Générer des palettes aléatoires"), this);
    boutonGenererPalettes->setIcon(QPixmap(":/Images/Engrenage.png"));
    boutonGenererPalettes->setIconSize(QSize(16, 16));
    connect(boutonGenererPalettes, SIGNAL(clicked()), this, SLOT(genererPalettesAleatoires()));
    connect(radioBoutonPalettes, SIGNAL(toggled(bool)), boutonGenererPalettes, SLOT(setEnabled(bool)));
    boutonGenererDegrades = new QPushButton(tr("Générer des dégradés aléatoires"), this);
    boutonGenererDegrades->setDisabled(true);
    boutonGenererDegrades->setIcon(QPixmap(":/Images/Engrenage.png"));
    boutonGenererDegrades->setIconSize(QSize(16, 16));
    connect(boutonGenererDegrades, SIGNAL(clicked()), this, SLOT(genererDegradesAleatoires()));
    connect(radioBoutonDegrades, SIGNAL(toggled(bool)), boutonGenererDegrades, SLOT(setEnabled(bool)));
    boutonParcourirFichierCommun = new QPushButton(tr("Parcourir un fichier commun"), this);
    boutonParcourirFichierCommun->setDisabled(true);
    boutonParcourirFichierCommun->setIcon(QPixmap(":/Images/Ouvrir.png"));
    boutonParcourirFichierCommun->setIconSize(QSize(16, 16));
    boutonParcourirFichierCommun->setDisabled(true);
    connect(boutonParcourirFichierCommun, SIGNAL(clicked()), this, SLOT(parcourirFichierCommun()));
    connect(radioBoutonFichiers, SIGNAL(toggled(bool)), boutonParcourirFichierCommun, SLOT(setEnabled(bool)));

    checkBoxAlphaZonesFixee = new QCheckBox(tr("Fixer la transparence des zones :"), this);
    spinBoxAlphaZonesFixee = new QSpinBox(this);
    spinBoxAlphaZonesFixee->setRange(1, 255);
    spinBoxAlphaZonesFixee->setValue(255);
    spinBoxAlphaZonesFixee->setDisabled(true);
    connect(checkBoxAlphaZonesFixee, SIGNAL(clicked(bool)), spinBoxAlphaZonesFixee, SLOT(setEnabled(bool)));

    checkBoxFondAvecImage = new QCheckBox(tr("Fond avec une image :"), this);
    fancyLineEditImageFond = new FancyLineEdit(QString(""), this);
    fancyLineEditImageFond->setPixmap(QPixmap(QString(":/Images/Ouvrir.png")));
    fancyLineEditImageFond->setAutoHideIcon(false);
    fancyLineEditImageFond->setReadOnly(true);
    fancyLineEditImageFond->iconButton()->setToolTip(tr("Parcourir"));
    fancyLineEditImageFond->disconnect();
    connect(fancyLineEditImageFond, SIGNAL(buttonClicked()), this, SLOT(parcourirImageFond()));
    fancyLineEditImageFond->setDisabled(true);
    connect(checkBoxFondAvecImage, SIGNAL(clicked(bool)), fancyLineEditImageFond, SLOT(setEnabled(bool)));
    checkBoxTailleAdapteeImage = new QCheckBox(tr("Taille du substrate adaptée à celle de l'image et au facteur d'échelle"), this);
    checkBoxTailleAdapteeImage->setDisabled(true);
    checkBoxTailleAdapteeImage->setChecked(true);
    connect(checkBoxFondAvecImage, SIGNAL(clicked(bool)), checkBoxTailleAdapteeImage, SLOT(setEnabled(bool)));

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(close()));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(tableWidget, 0, 0, 1, 5);
    gridLayout->addWidget(radioBoutonPalettes, 1, 0);
    gridLayout->addWidget(boutonGenererPalettes, 1, 1);
    gridLayout->addWidget(radioBoutonDegrades, 1, 3);
    gridLayout->addWidget(boutonGenererDegrades, 1, 4);
    gridLayout->addWidget(radioBoutonFichiers, 2, 0);
    gridLayout->addWidget(boutonParcourirFichierCommun, 2, 1);
    gridLayout->addWidget(checkBoxAlphaZonesFixee, 3, 0);
    gridLayout->addWidget(spinBoxAlphaZonesFixee, 3, 1);
    gridLayout->addWidget(checkBoxFondAvecImage, 4, 0);
    gridLayout->addWidget(fancyLineEditImageFond, 4, 1, 1, 4);
    gridLayout->addWidget(checkBoxTailleAdapteeImage, 5, 1, 1, 4);
    gridLayout->addWidget(frame, 6, 0, 1, 5);
    gridLayout->addWidget(dialogButtonBox, 7, 0, 1, 5);
    setLayout(gridLayout);

    resize(900, 375);

    setWindowTitle(tr("Configuration des couleurs"));
    setWindowIcon(QIcon(":/Images/Substrate.png"));

    enregistrerEtat();
}

void FenetreCouleurs::chargerSubstrate(Substrate *s) const
{
    for (int i = 0; i < NOMBRE_COULEURS; i++)
    {
        if (listeRadioBoutonsPalette[i]->isChecked())
            s->setCouleurs(Substrate::TypeCouleur(i), listeWidgetsPalette[i]->getListeCouleurs(), qobject_cast<QSpinBox *>(tableWidget->cellWidget(i, 1))->value());
        else if (listeRadioBoutonsDegrade[i]->isChecked())
            s->setCouleurs(Substrate::TypeCouleur(i), listeWidgetsDegrade[i]->getDegrade(), qobject_cast<QSpinBox *>(tableWidget->cellWidget(i, 1))->value());
        else
            s->setCouleurs(Substrate::TypeCouleur(i), listeFancyLineEdits[i]->text(), qobject_cast<QSpinBox *>(tableWidget->cellWidget(i, 1))->value());
        s->setAlphaZonesFixee(checkBoxAlphaZonesFixee->isChecked());
        s->setValeurAlphaZonesFixee(spinBoxAlphaZonesFixee->value());
        s->setPixmapFond(QPixmap(fancyLineEditImageFond->text()));
    }
}

void FenetreCouleurs::enregistrer(QDataStream &out) const
{
    out << listeMaxima;
    out << listePalettes;
    out << listeFichiers;
    out << listeChoixPalette;
    out << alphaZonesFixee;
    out << valeurAlphaZonesFixee;
    out << fondAvecImage;
    out << imageFond;
    out << tailleAdapteeImage;
    out << listeDegrades;
    out << listeChoixDegrade;
}

void FenetreCouleurs::charger(QDataStream &in, const QString &version)
{
    listeMaxima.clear();
    listePalettes.clear();
    listeFichiers.clear();
    listeChoixPalette.clear();
    listeDegrades.clear();
    listeChoixDegrade.clear();
    in >> listeMaxima;
    in >> listePalettes;
    in >> listeFichiers;
    in >> listeChoixPalette;
    in >> alphaZonesFixee;
    in >> valeurAlphaZonesFixee;
    in >> fondAvecImage;
    in >> imageFond;
    in >> tailleAdapteeImage;
    in >> listeDegrades;
    in >> listeChoixDegrade;
    annuler();
}

void FenetreCouleurs::ok()
{
    hide();
    enregistrerEtat();
}

void FenetreCouleurs::annuler()
{
    for (int i = 0; i < NOMBRE_COULEURS; i++)
    {
        qobject_cast<QSpinBox *>(tableWidget->cellWidget(i, 1))->setValue(listeMaxima[i]);
        listeWidgetsPalette[i]->setListeCouleurs(listePalettes[i]);
        listeWidgetsDegrade[i]->setDegrade(listeDegrades[i]);
        listeFancyLineEdits[i]->setText(listeFichiers[i]);
        listeRadioBoutonsPalette[i]->setChecked(listeChoixPalette[i]);
        choixCouleursChange(listeRadioBoutonsPalette[i]);
        listeRadioBoutonsDegrade[i]->setChecked(listeChoixDegrade[i]);
        choixCouleursChange(listeRadioBoutonsDegrade[i]);
        listeRadioBoutonsFichier[i]->setChecked(!listeRadioBoutonsPalette[i]->isChecked() && !listeRadioBoutonsDegrade[i]->isChecked());
        choixCouleursChange(listeRadioBoutonsFichier[i]);
        checkBoxAlphaZonesFixee->setChecked(alphaZonesFixee);
        spinBoxAlphaZonesFixee->setValue(valeurAlphaZonesFixee);
        checkBoxFondAvecImage->setChecked(fondAvecImage);
        fancyLineEditImageFond->setText(imageFond);
    }
    checkBoxAlphaZonesFixee->setChecked(alphaZonesFixee);
    spinBoxAlphaZonesFixee->setValue(valeurAlphaZonesFixee);
    checkBoxFondAvecImage->setChecked(fondAvecImage);
    fancyLineEditImageFond->setText(imageFond);
    checkBoxTailleAdapteeImage->setChecked(tailleAdapteeImage);
}

void FenetreCouleurs::parcourirFichier()
{
    FancyLineEdit *fancyLineEdit = qobject_cast<FancyLineEdit *>(sender());

    QString texte = QFileDialog::getOpenFileName(this, tr("Choisir un fichier"), fancyLineEdit->text(), tr("Images (*.bmp *.jpeg *.jpg *.png *.tiff)"));

    if (texte.isNull())
        return;

    fancyLineEdit->setText(texte);
}

void FenetreCouleurs::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    auto p{QGuiApplication::screenAt(pos())->geometry().center() - rect().center()};
    p.ry() -= 40;
    move(p);
}

void FenetreCouleurs::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);

    annuler();
}

void FenetreCouleurs::enregistrerEtat()
{
    listeMaxima.clear();
    listePalettes.clear();
    listeFichiers.clear();
    listeChoixPalette.clear();
    listeDegrades.clear();
    listeChoixDegrade.clear();
    for (int i = 0; i < NOMBRE_COULEURS; i++)
    {
        listeMaxima << qobject_cast<QSpinBox *>(tableWidget->cellWidget(i, 1))->value();
        listePalettes << listeWidgetsPalette[i]->getListeCouleurs();
        listeDegrades << listeWidgetsDegrade[i]->getDegrade();
        listeFichiers << listeFancyLineEdits[i]->text();
        listeChoixPalette << listeRadioBoutonsPalette[i]->isChecked();
        listeChoixDegrade << listeRadioBoutonsDegrade[i]->isChecked();
    }
    alphaZonesFixee = checkBoxAlphaZonesFixee->isChecked();
    valeurAlphaZonesFixee = spinBoxAlphaZonesFixee->value();
    fondAvecImage = checkBoxFondAvecImage->isChecked();
    imageFond = fancyLineEditImageFond->text();
    tailleAdapteeImage = checkBoxTailleAdapteeImage->isChecked();
}

void FenetreCouleurs::modifierPalette()
{
    int index = listeBoutonsPalette.indexOf(qobject_cast<QPushButton *>(sender()));
    Palette palette(index == 2 || index == 3 ? true : false, this);
    palette.setListeCouleurs(listeWidgetsPalette[index]->getListeCouleurs());

    if (palette.exec() == QDialog::Accepted)
        listeWidgetsPalette[index]->setListeCouleurs(palette.getListeCouleurs());
}

void FenetreCouleurs::genererPaletteAleatoire()
{
    int index = listeBoutonsGenererPalette.indexOf(qobject_cast<QPushButton *>(sender()));

    listeWidgetsPalette[index]->genererCouleurs(index == 2 || index == 3 ? true : false);
}

void FenetreCouleurs::genererPalettesAleatoires()
{
    for (int i = 0; i < NOMBRE_COULEURS; i++)
        if (listeWidgetsPalette[i]->isEnabled())
            listeWidgetsPalette[i]->genererCouleurs(i == 2 || i == 3 ? true : false);
}

void FenetreCouleurs::modifierDegrade()
{
    int index = listeBoutonsDegrade.indexOf(qobject_cast<QPushButton *>(sender()));
    DialogueDegrade dialogueDegrade(index == 2 || index == 3 ? true : false, this);
    dialogueDegrade.setDegrade(listeWidgetsDegrade[index]->getDegrade());

    if (dialogueDegrade.exec() == QDialog::Accepted)
        listeWidgetsDegrade[index]->setDegrade(dialogueDegrade.getDegrade());
}

void FenetreCouleurs::genererDegradeAleatoire()
{
    int index = listeBoutonsGenererDegrade.indexOf(qobject_cast<QPushButton *>(sender()));

    listeWidgetsDegrade[index]->genererCouleurs(index == 2 || index == 3 ? true : false);
}

void FenetreCouleurs::genererDegradesAleatoires()
{
    for (int i = 0; i < NOMBRE_COULEURS; i++)
        if (listeWidgetsDegrade[i]->isEnabled())
            listeWidgetsDegrade[i]->genererCouleurs(i == 2 || i == 3 ? true : false);
}

void FenetreCouleurs::parcourirFichierCommun()
{
    QString texte("pollockShimmering.gif");

    if (tableWidget->currentRow() != -1)
        texte = listeFancyLineEdits[tableWidget->currentRow()]->text();

    texte = QFileDialog::getOpenFileName(this, tr("Choisir un fichier"), texte, tr("Images (*.bmp *.jpeg *.jpg *.png *.tiff)"));

    if (texte.isNull())
        return;

    for (int i = 0; i < NOMBRE_COULEURS; i++)
        if (listeFancyLineEdits[i]->isEnabled())
            listeFancyLineEdits[i]->setText(texte);
}

void FenetreCouleurs::parcourirImageFond()
{
    QString texte = QFileDialog::getOpenFileName(this, tr("Choisir un fichier"), fancyLineEditImageFond->text(), tr("Images (*.bmp *.jpeg *.jpg *.png *.tiff)"));

    if (!texte.isNull())
        fancyLineEditImageFond->setText(texte);
}

void FenetreCouleurs::choixCouleursChange(QAbstractButton *bouton)
{
    if (traitementEnCours)
        return;
    int compteurPalettes = 0, compteurDegrades = 0;
    for (int i = 0; i < NOMBRE_COULEURS; i++)
    {
        if (listeRadioBoutonsPalette[i]->isChecked())
            compteurPalettes++;
        else if (listeRadioBoutonsDegrade[i]->isChecked())
            compteurDegrades++;
    }
    radioBoutonPalettes->blockSignals(true);
    radioBoutonDegrades->blockSignals(true);
    radioBoutonFichiers->blockSignals(true);
    boutonGenererPalettes->setDisabled(!compteurPalettes);
    boutonGenererDegrades->setDisabled(!compteurDegrades);
    boutonParcourirFichierCommun->setDisabled(compteurPalettes+compteurDegrades == NOMBRE_COULEURS);
    groupeBoutons->setExclusive(false);
    radioBoutonPalettes->setChecked(false);
    radioBoutonDegrades->setChecked(false);
    radioBoutonFichiers->setChecked(false);
    if (compteurPalettes == NOMBRE_COULEURS)
        radioBoutonPalettes->setChecked(true);
    else if (compteurDegrades == NOMBRE_COULEURS)
        radioBoutonDegrades->setChecked(true);
    else if (!compteurDegrades && !compteurPalettes)
        radioBoutonFichiers->setChecked(true);
    groupeBoutons->setExclusive(true);
    radioBoutonPalettes->blockSignals(false);
    radioBoutonDegrades->blockSignals(false);
    radioBoutonFichiers->blockSignals(false);
}

void FenetreCouleurs::choixToutesCouleursChange(QAbstractButton *bouton)
{
    if (!bouton->isChecked())
        return;
    traitementEnCours = true;
    for (int i = 0; i < NOMBRE_COULEURS; i++)
    {
        if (bouton == radioBoutonPalettes)
            listeRadioBoutonsPalette[i]->setChecked(true);
        else if (bouton == radioBoutonDegrades)
            listeRadioBoutonsDegrade[i]->setChecked(true);
        else
            listeRadioBoutonsFichier[i]->setChecked(true);
    }
    traitementEnCours = false;
}

QString FenetreCouleurs::getImageFond() const
{
    return fancyLineEditImageFond->text();
}

bool FenetreCouleurs::getTailleAdapteeImage() const
{
    return checkBoxTailleAdapteeImage->isChecked();
}
