#include <QApplication>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QRegularExpressionValidator>
#include <QScreen>

#include "constantes.h"
#include "FancyLineEdit.h"
#include "FenetreGenerateurs.h"
#include "Substrate.h"

FenetreGenerateurs::FenetreGenerateurs(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    tableWidget = new QTableWidget(NOMBRE_GENERATEURS, 2, this);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Générateur") << tr("Clé"));

    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        QCheckBox *checkBox = new QCheckBox(this);
        tableWidget->setCellWidget(i, 0, checkBox);
        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(forcageChange(int)));
        FancyLineEdit *fancyLineEdit = new FancyLineEdit(this);
        fancyLineEdit->setPixmap(QPixmap(QString(":/Images/Engrenage.png")));
        fancyLineEdit->setAutoHideIcon(false);
        fancyLineEdit->setToolTip(tr("Générer une clé aléatoire"));
        QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression(QString("[a-fA-F0-9]{1,16}")), fancyLineEdit);
        fancyLineEdit->setValidator(validator);
        fancyLineEdit->disconnect();
        connect(fancyLineEdit, SIGNAL(buttonClicked()), this, SLOT(genererCleAleatoire()));
        fancyLineEdit->iconClicked();
        fancyLineEdit->setDisabled(true);
        tableWidget->setCellWidget(i, 1, fancyLineEdit);
    }

    qobject_cast<QCheckBox *>(tableWidget->cellWidget(0, 0))->setText(tr("Cracks"));
    qobject_cast<QCheckBox *>(tableWidget->cellWidget(1, 0))->setText(tr("Sable"));
    qobject_cast<QCheckBox *>(tableWidget->cellWidget(2, 0))->setText(tr("Couleur des cracks"));
    qobject_cast<QCheckBox *>(tableWidget->cellWidget(3, 0))->setText(tr("Couleur du sable"));
    qobject_cast<QCheckBox *>(tableWidget->cellWidget(4, 0))->setText(tr("Couleur des zones"));
    qobject_cast<QCheckBox *>(tableWidget->cellWidget(5, 0))->setText(tr("Couleur du fond"));
    qobject_cast<QCheckBox *>(tableWidget->cellWidget(6, 0))->setText(tr("Transparence des zones"));

    tableWidget->resizeRowsToContents();
    tableWidget->resizeColumnsToContents();

    checkBoxForcage = new QCheckBox(tr("Forcer les générateurs"), this);
    checkBoxForcage->setTristate(true);
    checkBoxForcage->setToolTip(tr("Forcer les clés des générateurs"));
    connect(checkBoxForcage, SIGNAL(stateChanged(int)), this, SLOT(forcerGenerateurs(int)));
    boutonGenerer = new QPushButton(tr("Générer des clés"), this);
    boutonGenerer->setIcon(QPixmap(":/Images/Engrenage.png"));
    boutonGenerer->setToolTip(tr("Générer des clés aléatoires pour les générateurs forcés"));
    boutonGenerer->setDisabled(true);
    connect(boutonGenerer, SIGNAL(clicked()), this, SLOT(genererClesAleatoires()));

    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this);
    QPushButton *boutonOk = dialogButtonBox->button(QDialogButtonBox::Ok);
    QPushButton *boutonAnnuler = dialogButtonBox->button(QDialogButtonBox::Cancel);
    connect(boutonOk, SIGNAL(clicked()), this, SLOT(ok()));
    connect(boutonAnnuler, SIGNAL(clicked()), this, SLOT(close()));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(tableWidget, 0, 0, 6, 1);
    gridLayout->addWidget(checkBoxForcage, 1, 1);
    gridLayout->addWidget(boutonGenerer, 2, 1);
    gridLayout->addWidget(boutonOk, 4, 1);
    gridLayout->addWidget(boutonAnnuler, 5, 1);
    setLayout(gridLayout);

    delete dialogButtonBox;

    resize(600, 250);

    setWindowTitle(tr("Configuration des générateurs"));
    setWindowIcon(QIcon(":/Images/Substrate.png"));

    enregistrerEtat();
}

void FenetreGenerateurs::chargerSubstrate(Substrate *s) const
{
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        s->setGenerateurForce(Substrate::TypeGenerateur(i), qobject_cast<QCheckBox *>(tableWidget->cellWidget(i, 0))->isChecked());
        s->setCleGenerateur(Substrate::TypeGenerateur(i), qobject_cast<FancyLineEdit *>(tableWidget->cellWidget(i, 1))->text().toULongLong(0, 16));
    }
    s->initialiserGenerateurs();
}

void FenetreGenerateurs::chargerClesSubstrate(Substrate *s)
{
    listeForcages.clear();
    listeCles.clear();
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        listeForcages << s->getGenerateurForce(Substrate::TypeGenerateur(i));
        listeCles << QString::number(s->getCleGenerateur(Substrate::TypeGenerateur(i)), 16).rightJustified(16, '0');
        qobject_cast<QCheckBox *>(tableWidget->cellWidget(i, 0))->setChecked(listeForcages.last());
        qobject_cast<FancyLineEdit *>(tableWidget->cellWidget(i, 1))->setText(listeCles.last());
    }
}

void FenetreGenerateurs::enregistrer(QDataStream &out) const
{
    out << listeForcages;
    out << listeCles;
}

void FenetreGenerateurs::charger(QDataStream &in, const QString &version)
{
    listeForcages.clear();
    listeCles.clear();
    in >> listeForcages;
    in >> listeCles;
    annuler();
}

void FenetreGenerateurs::genererCleAleatoire()
{
    qobject_cast<FancyLineEdit *>(sender())->setText(QString::number((quint64)(((quint64)(rand()%12345) << 48) | ((quint64)(rand()%23451) << 32) | ((quint64)(rand()%34512) << 16) | (quint64)(rand()%45123)), 16).rightJustified(16, '0'));
}

void FenetreGenerateurs::ok()
{
    hide();
    enregistrerEtat();
}

void FenetreGenerateurs::annuler()
{
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        qobject_cast<QCheckBox *>(tableWidget->cellWidget(i, 0))->setChecked(listeForcages[i]);
        qobject_cast<FancyLineEdit *>(tableWidget->cellWidget(i, 1))->setText(listeCles[i]);
        tableWidget->cellWidget(i, 1)->setDisabled(!listeForcages[i]);
    }
}

void FenetreGenerateurs::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    auto p{QGuiApplication::screenAt(pos())->geometry().center() - rect().center()};
    p.ry() -= 40;
    move(p);
}

void FenetreGenerateurs::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);

    annuler();
}

void FenetreGenerateurs::enregistrerEtat()
{
    listeCles.clear();
    listeForcages.clear();
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        listeForcages << qobject_cast<QCheckBox *>(tableWidget->cellWidget(i, 0))->isChecked();
        listeCles << qobject_cast<FancyLineEdit *>(tableWidget->cellWidget(i, 1))->text().rightJustified(16, '0');
    }
}

void FenetreGenerateurs::forcageChange(int etat)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    int compteur = 0;
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        QCheckBox *c = qobject_cast<QCheckBox *>(tableWidget->cellWidget(i, 0));
        if (c->isChecked())
            compteur++;
        if (c == checkBox)
            tableWidget->cellWidget(i, 1)->setDisabled(!etat);
    }
    checkBoxForcage->blockSignals(true);
    if (!compteur || compteur == NOMBRE_GENERATEURS)
        checkBoxForcage->setCheckState(Qt::CheckState(etat));
    else
        checkBoxForcage->setCheckState(Qt::PartiallyChecked);
    checkBoxForcage->blockSignals(false);
    if (checkBoxForcage->checkState() == Qt::Checked)
    {
        checkBoxForcage->setText(tr("Déforcer les générateurs"));
        checkBoxForcage->setToolTip(tr("Déforcer les clés des générateurs"));
    }
    else
    {
        checkBoxForcage->setText(tr("Forcer les générateurs"));
        checkBoxForcage->setToolTip(tr("Forcer les clés des générateurs"));
    }
}

void FenetreGenerateurs::forcerGenerateurs(int etat)
{
    if (etat == Qt::PartiallyChecked)
    {
        qobject_cast<QCheckBox *>(sender())->setCheckState(Qt::Checked);
        return;
    }
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        QCheckBox *c = qobject_cast<QCheckBox *>(tableWidget->cellWidget(i, 0));
        c->blockSignals(true);
        c->setCheckState(Qt::CheckState(etat));
        c->blockSignals(false);
        tableWidget->cellWidget(i, 1)->setDisabled(!etat);
    }
    if (Qt::CheckState(etat) == Qt::Checked)
    {
        checkBoxForcage->setText(tr("Déforcer les générateurs"));
        checkBoxForcage->setToolTip(tr("Déforcer les clés des générateurs"));
        boutonGenerer->setDisabled(false);
    }
    else
    {
        checkBoxForcage->setText(tr("Forcer les générateurs"));
        checkBoxForcage->setToolTip(tr("Forcer les clés des générateurs"));
        boutonGenerer->setDisabled(true);
    }
}

void FenetreGenerateurs::genererClesAleatoires()
{
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        FancyLineEdit *fancyLineEdit = qobject_cast<FancyLineEdit *>(tableWidget->cellWidget(i, 1));
        if (fancyLineEdit->isEnabled())
            fancyLineEdit->setText(QString::number((quint64)(((quint64)(rand()%12345) << 48) | ((quint64)(rand()%23451) << 32) | ((quint64)(rand()%34512) << 16) | (quint64)(rand()%45123)), 16).rightJustified(16, '0'));
    }
}
