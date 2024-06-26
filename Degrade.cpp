#include <QSpinBox>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QInputDialog>
#include <QPainter>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>

#include "Degrade.h"

QDataStream &operator<<(QDataStream &out, const Degrade &degrade)
{
    return out << degrade.couleurDepart << degrade.listeCouleurs;
}

QDataStream &operator>>(QDataStream &in, Degrade &degrade)
{
    return in >> degrade.couleurDepart >> degrade.listeCouleurs;
}

bool operator==(const Degrade &d1, const Degrade &d2)
{
    return (d1.couleurDepart == d2.couleurDepart && d1.listeCouleurs == d2.listeCouleurs);
}

WidgetDegrade::WidgetDegrade(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    Degrade degradeDefaut;
    degradeDefaut.couleurDepart = Qt::white;
    degradeDefaut.listeCouleurs << QPair<int, QColor>(128, Qt::black);

    setDegrade(degradeDefaut);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

const Degrade & WidgetDegrade::getDegrade() const
{
    return degrade;
}

void WidgetDegrade::setDegrade(const Degrade &d)
{
    degrade = d;

    update();
}

int WidgetDegrade::longueurOptimale() const
{
    int nombre = 0;

    for (int i = 0; i < degrade.listeCouleurs.size(); i++)
        nombre += degrade.listeCouleurs[i].first;

    return nombre;
}

void WidgetDegrade::genererCouleurs(bool editionCanalAlpha)
{
    degrade.couleurDepart = QColor(rand()%256, rand()%256, rand()%256);
    if (editionCanalAlpha)
        degrade.couleurDepart.setAlpha(rand()%256);

    for (int i = 0; i < degrade.listeCouleurs.size(); i++)
    {
        degrade.listeCouleurs[i].second = QColor(rand()%256, rand()%256, rand()%256);
        if (editionCanalAlpha)
            degrade.listeCouleurs[i].second.setAlpha(rand()%256);
    }

    update();
}

void WidgetDegrade::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);

    int nombre = 0;

    for (int i = 0; i < degrade.listeCouleurs.size(); i++)
        nombre += degrade.listeCouleurs[i].first;

    qreal sx = (qreal)width()/(qreal)nombre;

    painter.scale(sx, 1);

    int x = 0;

    QLinearGradient linearGradient;
    linearGradient.setColorAt(0, degrade.couleurDepart);

    for (int i = 0; i < degrade.listeCouleurs.size(); i++)
    {
        linearGradient.setStart(x, 0);
        linearGradient.setColorAt(1, degrade.listeCouleurs[i].second);
        linearGradient.setFinalStop(x+degrade.listeCouleurs[i].first, 0);
        painter.fillRect(x, 0, degrade.listeCouleurs[i].first, height(), linearGradient);
        x += degrade.listeCouleurs[i].first;
        linearGradient.setColorAt(0, degrade.listeCouleurs[i].second);
    }

    painter.scale(1.0/sx, 1);

    if (!isEnabled())
    {
        QColor c = palette().color(QPalette::Disabled, QPalette::Window);
        c.setAlpha(128);
        painter.fillRect(rect(), c);
    }
}

DialogueDegrade::DialogueDegrade(bool editionCanalAlpha, QWidget *parent) : QDialog(parent)
{
    m_editionCanalAlpha = editionCanalAlpha;
    rafraichirEnFermant = false;

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels(QStringList() << tr("Longueur") << tr("Code hexadécimal") << tr("Couleur"));
    treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeWidget->setRootIsDecorated(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    treeWidget->setMinimumWidth(350);

    boutonAjouter = new QPushButton(tr("&Ajouter une couleur"), this);
    boutonInserer = new QPushButton(tr("&Insérer une couleur"), this);
    boutonModifier = new QPushButton(tr("&Modifier une couleur"), this);
    boutonSupprimer = new QPushButton(tr("&Supprimer une couleur"), this);
    boutonGenerer = new QPushButton(tr("&Générer un\ndégradé aléatoire"), this);
    boutonCouleurDepart = new QPushButton(this);

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    boutonOk = dialogButtonBox->button(QDialogButtonBox::Ok);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(treeWidget, 0, 0, 5, 3);
    gridLayout->addWidget(boutonAjouter, 0, 3);
    gridLayout->addWidget(boutonInserer, 1, 3);
    gridLayout->addWidget(boutonModifier, 2, 3);
    gridLayout->addWidget(boutonSupprimer, 3, 3);
    gridLayout->addWidget(boutonGenerer, 4, 3);
    gridLayout->addWidget(frame, 5, 0, 1, 4);
    gridLayout->addWidget(new QLabel(tr("Couleur de départ :"), this), 6, 0);
    gridLayout->addWidget(boutonCouleurDepart, 6, 1);
    gridLayout->addWidget(dialogButtonBox, 6, 3);
    setLayout(gridLayout);

    connect(boutonAjouter, SIGNAL(clicked()), this, SLOT(ajouterCouleur()));
    connect(boutonInserer, SIGNAL(clicked()), this, SLOT(insererCouleur()));
    connect(boutonModifier, SIGNAL(clicked()), this, SLOT(modifierCouleur()));
    connect(boutonSupprimer, SIGNAL(clicked()), this, SLOT(supprimerCouleur()));
    connect(boutonGenerer, SIGNAL(clicked()), this, SLOT(genererCouleurs()));
    connect(boutonCouleurDepart, SIGNAL(clicked()), this, SLOT(modifierCouleurDepart()));
    connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(annuler()));

    setWindowTitle(tr("Degradé de couleurs"));

    Degrade degradeDefaut;
    degradeDefaut.couleurDepart = Qt::white;
    degradeDefaut.listeCouleurs << QPair<int, QColor>(128, Qt::black);

    setDegrade(degradeDefaut);
}

const Degrade & DialogueDegrade::getDegrade() const
{
    return degrade;
}

void DialogueDegrade::setDegrade(const Degrade &d)
{
    if (degrade == d)
        return;

    degrade = d;

    treeWidget->clear();
    treeWidget->hide();

    QPalette p = boutonCouleurDepart->palette();
    p.setColor(QPalette::Button, degrade.couleurDepart);
    boutonCouleurDepart->setPalette(p);
    boutonCouleurDepart->setIconSize(QSize(64, 16));
    QPixmap pixmap(64, 16);
    pixmap.fill(degrade.couleurDepart);
    boutonCouleurDepart->setIcon(QIcon(pixmap));
    boutonCouleurDepart->setProperty("couleur", degrade.couleurDepart);

    for (int i = 0; i < degrade.listeCouleurs.size(); i++)
        creerItem(degrade.listeCouleurs[i]);

    treeWidget->show();
}

void DialogueDegrade::ajouterCouleur()
{
    QColor couleur;
    if (m_editionCanalAlpha)
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"), QColorDialog::ShowAlphaChannel);
    else
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"));

    if (!couleur.isValid())
        return;

    degrade.listeCouleurs << QPair<int, QColor>(128, couleur);
    creerItem(degrade.listeCouleurs.last());
}

void DialogueDegrade::insererCouleur()
{
    QColor couleur;

    if (m_editionCanalAlpha)
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"), QColorDialog::ShowAlphaChannel);
    else
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"));

    if (!couleur.isValid())
        return;

    creerItem(QPair<int, QColor>(128, couleur), treeWidget->indexOfTopLevelItem(treeWidget->currentItem()));
}

void DialogueDegrade::modifierCouleur()
{
    int index = treeWidget->indexOfTopLevelItem(treeWidget->currentItem());

    if (sender())
    {
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
        {
            if (qobject_cast<QWidget *>(sender()) == treeWidget->itemWidget(treeWidget->topLevelItem(i), 2))
            {
                index = i;
                break;
            }
        }
    }

    if (index != -1)
    {
        QColor couleur;

        if (m_editionCanalAlpha)
            couleur = QColorDialog::getColor(QColor::fromRgba(treeWidget->topLevelItem(index)->data(1, Qt::DisplayRole).toString().remove("#").toUInt(0, 16)), this, tr("Choisir une couleur"), QColorDialog::ShowAlphaChannel);
        else
            couleur = QColorDialog::getColor(QColor::fromRgb(treeWidget->topLevelItem(index)->data(1, Qt::DisplayRole).toString().remove("#").toUInt(0, 16)), this, tr("Choisir une couleur"));

        if (!couleur.isValid())
            return;

        treeWidget->topLevelItem(index)->setData(1, Qt::DisplayRole, tr("#")+QString::number(m_editionCanalAlpha ? couleur.rgba() : couleur.rgb(), 16));
        QPushButton *b = qobject_cast<QPushButton *>(treeWidget->itemWidget(treeWidget->topLevelItem(index), 2));
        QPalette p = b->palette();
        p.setColor(QPalette::Button, couleur);
        b->setPalette(p);
        b->setIconSize(QSize(64, 16));
        QPixmap pixmap(64, 16);
        pixmap.fill(couleur);
        b->setIcon(QIcon(pixmap));
    }
}

void DialogueDegrade::supprimerCouleur()
{
    treeWidget->hide();
    QList<QTreeWidgetItem *> l = treeWidget->selectedItems();
    for (int i = 0; i < l.size(); i++)
    {
        treeWidget->removeItemWidget(l[i], 0);
        treeWidget->removeItemWidget(l[i], 2);
        delete treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(l[i]));
    }
    treeWidget->show();
    boutonOk->setDisabled(!treeWidget->topLevelItemCount());
}

void DialogueDegrade::genererCouleurs()
{
    bool ok;
    int nb = QInputDialog::getInt(this, tr("Génération aléatoire des couleurs"), tr("Combien de couleurs doivent être générées ?"), 2, 2, 2147483647, 1, &ok);

    if (!ok)
        return;

    treeWidget->clear();
    treeWidget->hide();

    QColor couleur = QColor(rand()%256, rand()%256, rand()%256);
    if (m_editionCanalAlpha)
        couleur.setAlpha(rand()%256);

    QPalette p = boutonCouleurDepart->palette();
    p.setColor(QPalette::Button, couleur);
    boutonCouleurDepart->setPalette(p);
    boutonCouleurDepart->setIconSize(QSize(64, 16));
    QPixmap pixmap(64, 16);
    pixmap.fill(couleur);
    boutonCouleurDepart->setIcon(QIcon(pixmap));

    for (int i = 0; i < nb; i++)
    {
        QPair<int, QColor> pair(128, QColor(rand()%256, rand()%256, rand()%256));
        if (m_editionCanalAlpha)
            pair.second.setAlpha(rand()%256);
        creerItem(pair);
    }

    treeWidget->show();
}

void DialogueDegrade::creerItem(const QPair<int, QColor> &pair, int index)
{
    if (index == -1)
        index = treeWidget->topLevelItemCount();
    QTreeWidgetItem *item = new QTreeWidgetItem;
    treeWidget->insertTopLevelItem(index, item);
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setRange(1, QWIDGETSIZE_MAX);
    spinBox->setValue(pair.first);
    QPushButton *bouton = new QPushButton;
    bouton->setIconSize(QSize(64, 16));
    QPixmap pixmap(64, 16);
    pixmap.fill(pair.second);
    bouton->setIcon(QIcon(pixmap));
    QPalette p = bouton->palette();
    p.setColor(QPalette::Button, pair.second);
    bouton->setPalette(p);
    connect(bouton, SIGNAL(clicked()), this, SLOT(modifierCouleur()));
    treeWidget->setItemWidget(item, 0, spinBox);
    item->setData(1, Qt::DisplayRole, tr("#")+QString::number(m_editionCanalAlpha ? pair.second.rgba() : pair.second.rgb(), 16));
    treeWidget->setItemWidget(item, 2, bouton);
    boutonOk->setDisabled(false);
}

void DialogueDegrade::ok()
{
    degrade.couleurDepart = boutonCouleurDepart->property("couleur").value<QColor>();

    degrade.listeCouleurs.clear();

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        QPair<int, QColor> pair;
        pair.first = qobject_cast<QSpinBox *>(treeWidget->itemWidget(treeWidget->topLevelItem(i), 0))->value();
        if (m_editionCanalAlpha)
            pair.second = QColor::fromRgba(treeWidget->topLevelItem(i)->data(1, Qt::DisplayRole).toString().remove("#").toUInt(0, 16));
        else
            pair.second = QColor::fromRgb(treeWidget->topLevelItem(i)->data(1, Qt::DisplayRole).toString().remove("#").toUInt(0, 16));
        degrade.listeCouleurs << pair;
    }

    accept();
}

void DialogueDegrade::annuler()
{
    close();

    treeWidget->clear();

    if (rafraichirEnFermant)
    {
        QPalette p = boutonCouleurDepart->palette();
        p.setColor(QPalette::Button, degrade.couleurDepart);
        boutonCouleurDepart->setPalette(p);
        boutonCouleurDepart->setIconSize(QSize(64, 16));
        QPixmap pixmap(64, 16);
        pixmap.fill(degrade.couleurDepart);
        boutonCouleurDepart->setIcon(QIcon(pixmap));
        boutonCouleurDepart->setProperty("couleur", degrade.couleurDepart);

        treeWidget->hide();

        for (int i = 0; i < degrade.listeCouleurs.size(); i++)
            creerItem(degrade.listeCouleurs[i]);

        treeWidget->show();
    }

    reject();
}

bool DialogueDegrade::getRafraichirEnFermant() const
{
    return rafraichirEnFermant;
}

void DialogueDegrade::setRafraichirEnFermant(bool b)
{
    rafraichirEnFermant = b;
}

void DialogueDegrade::modifierCouleurDepart()
{
    QColor couleur = boutonCouleurDepart->property("couleur").value<QColor>();

    if (m_editionCanalAlpha)
        couleur = QColorDialog::getColor(couleur, this, tr("Choisir une couleur"), QColorDialog::ShowAlphaChannel);
    else
        couleur = QColorDialog::getColor(couleur, this, tr("Choisir une couleur"));

    if (!couleur.isValid())
        return;

    QPalette p = boutonCouleurDepart->palette();
    p.setColor(QPalette::Button, couleur);
    boutonCouleurDepart->setPalette(p);
    boutonCouleurDepart->setIconSize(QSize(64, 16));
    QPixmap pixmap(64, 16);
    pixmap.fill(couleur);
    boutonCouleurDepart->setIcon(QIcon(pixmap));
    boutonCouleurDepart->setProperty("couleur", couleur);
}
