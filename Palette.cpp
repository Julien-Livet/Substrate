#include <QDialogButtonBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QColorDialog>
#include <QFrame>
#include <QHeaderView>
#include <QPainter>
#include <ctime>

#include "Palette.h"

extern QStringList listeCouleursDefaut;

QDataStream &operator<<(QDataStream &out, const Couleur &couleur)
{
    return out << couleur.couleur << couleur.nombre;
}

QDataStream &operator>>(QDataStream &in, Couleur &couleur)
{
    return in >> couleur.couleur >> couleur.nombre;
}

bool operator==(const Couleur &c1, const Couleur &c2)
{
    return (c1.couleur == c2.couleur);
}

WidgetPalette::WidgetPalette(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    setListeCouleurs(listeCouleursDefaut);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

const QList<Couleur> & WidgetPalette::getListeCouleurs() const
{
    return listeCouleurs;
}

void WidgetPalette::setListeCouleurs(const QList<QColor> &liste)
{
    listeCouleurs.clear();

    Couleur c;

    for (int i = 0; i < liste.size(); i++)
    {
        c.couleur = liste[i];
        if (!listeCouleurs.contains(c))
        {
            c.nombre = liste.count(liste[i]);
            listeCouleurs << c;
        }
    }

    update();
}

void WidgetPalette::setListeCouleurs(const QList<Couleur> &liste)
{
    if (listeCouleurs == liste)
        return;

    listeCouleurs = liste;

    update();
}

void WidgetPalette::setListeCouleurs(const QStringList &liste)
{
    listeCouleurs.clear();

    Couleur couleur;
    couleur.nombre = 1;

    for (int i = 0; i < liste.size(); i++)
    {
        QColor c = QColor::fromRgb(QString(liste[i]).remove("#").toUInt(0, 16));
        couleur.couleur = c;
        if (listeCouleurs.contains(couleur))
            listeCouleurs[listeCouleurs.indexOf(couleur)].nombre++;
        else
            listeCouleurs << couleur;
    }

    update();
}

int WidgetPalette::longueurOptimale() const
{
    int nombre = 0;

    for (int i = 0; i < listeCouleurs.size(); i++)
        nombre += listeCouleurs[i].nombre;

    return nombre;
}

void WidgetPalette::genererCouleurs(bool editionCanalAlpha)
{
    QList<QColor> l;
    QColor couleur;

    for (int i = 0; i < listeCouleurs.size(); i++)
    {
        do
        {
            couleur = QColor(rand()%256, rand()%256, rand()%256);
            if (editionCanalAlpha)
                couleur.setAlpha(rand()%256);
        } while (l.contains(couleur));
        l << couleur;
        listeCouleurs[i].couleur = couleur;
    }

    update();
}

void WidgetPalette::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);

    int nombre = 0;

    for (int i = 0; i < listeCouleurs.size(); i++)
        nombre += listeCouleurs[i].nombre;

    qreal sx = (qreal)width()/(qreal)nombre;

    painter.scale(sx, 1);

    int x = 0;

    for (int i = 0; i < listeCouleurs.size(); i++)
    {
        painter.fillRect(x, 0, listeCouleurs[i].nombre, height(), listeCouleurs[i].couleur);
        x += listeCouleurs[i].nombre;
    }

    painter.scale(1.0/sx, 1);

    if (!isEnabled())
    {
        QColor c = palette().color(QPalette::Disabled, QPalette::Window);
        c.setAlpha(128);
        painter.fillRect(rect(), c);
    }
}

Palette::Palette(bool editionCanalAlpha, QWidget *parent) : QDialog(parent)
{
    m_editionCanalAlpha = editionCanalAlpha;
    rafraichirEnFermant = false;

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels(QStringList() << tr("Code hexadécimal") << tr("Couleur") << tr("Nombre"));
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
    boutonGenerer = new QPushButton(tr("&Générer une\npalette aléatoire"), this);

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    boutonOk = dialogButtonBox->button(QDialogButtonBox::Ok);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(treeWidget, 0, 0, 5, 1);
    gridLayout->addWidget(boutonAjouter, 0, 1);
    gridLayout->addWidget(boutonInserer, 1, 1);
    gridLayout->addWidget(boutonModifier, 2, 1);
    gridLayout->addWidget(boutonSupprimer, 3, 1);
    gridLayout->addWidget(boutonGenerer, 4, 1);
    gridLayout->addWidget(frame, 5, 0, 1 ,2);
    gridLayout->addWidget(dialogButtonBox, 6, 0, 1, 2);
    setLayout(gridLayout);

    connect(boutonAjouter, SIGNAL(clicked()), this, SLOT(ajouterCouleur()));
    connect(boutonInserer, SIGNAL(clicked()), this, SLOT(insererCouleur()));
    connect(boutonModifier, SIGNAL(clicked()), this, SLOT(modifierCouleur()));
    connect(boutonSupprimer, SIGNAL(clicked()), this, SLOT(supprimerCouleur()));
    connect(boutonGenerer, SIGNAL(clicked()), this, SLOT(genererCouleurs()));
    connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(annuler()));

    setWindowTitle(tr("Palette de couleurs"));
}

const QList<Couleur> & Palette::getListeCouleurs() const
{
    return listeCouleurs;
}

void Palette::setListeCouleurs(const QList<Couleur> &liste)
{
    if (listeCouleurs == liste)
        return;

    treeWidget->clear();
    treeWidget->hide();

    for (int i = 0; i < liste.size(); i++)
    {
        listeCouleurs << liste[i];
        creerItem(listeCouleurs.last());
    }

    treeWidget->show();
}

void Palette::ajouterCouleur()
{
    QColor couleur;
    if (m_editionCanalAlpha)
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"), QColorDialog::ShowAlphaChannel);
    else
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"));

    if (!couleur.isValid())
        return;

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        if (treeWidget->topLevelItem(i)->data(0, Qt::DisplayRole) == tr("#")+QString::number(m_editionCanalAlpha ? couleur.rgba() : couleur.rgb(), 16))
        {
            QSpinBox *s = qobject_cast<QSpinBox *>(treeWidget->itemWidget(treeWidget->topLevelItem(i), 2));
            s->setValue(s->value()+1);
            return;
        }
    }

    Couleur c;
    c.couleur = couleur;
    c.nombre = 1;
    creerItem(c);
}

void Palette::insererCouleur()
{
    QColor couleur;

    if (m_editionCanalAlpha)
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"), QColorDialog::ShowAlphaChannel);
    else
        couleur = QColorDialog::getColor(QColor(255, 255, 255), this, tr("Choisir une couleur"));

    if (!couleur.isValid())
        return;

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        if (treeWidget->topLevelItem(i)->data(0, Qt::DisplayRole) == tr("#")+QString::number(m_editionCanalAlpha ? couleur.rgba() : couleur.rgb(), 16))
        {
            QSpinBox *s = qobject_cast<QSpinBox *>(treeWidget->itemWidget(treeWidget->topLevelItem(i), 2));
            s->setValue(s->value()+1);
            return;
        }
    }

    Couleur c;
    c.couleur = couleur;
    c.nombre = 1;
    creerItem(c, treeWidget->indexOfTopLevelItem(treeWidget->currentItem()));
}

void Palette::modifierCouleur()
{
    int index = treeWidget->indexOfTopLevelItem(treeWidget->currentItem());

    if (sender())
    {
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
        {
            if (qobject_cast<QWidget *>(sender()) == treeWidget->itemWidget(treeWidget->topLevelItem(i), 1))
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
            couleur = QColorDialog::getColor(QColor::fromRgba(treeWidget->topLevelItem(index)->data(0, Qt::DisplayRole).toString().remove("#").toUInt(0, 16)), this, tr("Choisir une couleur"), QColorDialog::ShowAlphaChannel);
        else
            couleur = QColorDialog::getColor(QColor::fromRgb(treeWidget->topLevelItem(index)->data(0, Qt::DisplayRole).toString().remove("#").toUInt(0, 16)), this, tr("Choisir une couleur"));

        if (!couleur.isValid())
            return;

        treeWidget->topLevelItem(index)->setData(0, Qt::DisplayRole, tr("#")+QString::number(m_editionCanalAlpha ? couleur.rgba() : couleur.rgb(), 16));
        QPushButton *b = qobject_cast<QPushButton *>(treeWidget->itemWidget(treeWidget->topLevelItem(index), 1));
        QPalette p = b->palette();
        p.setColor(QPalette::Button, couleur);
        b->setPalette(p);
        b->setIconSize(QSize(64, 16));
        QPixmap pixmap(64, 16);
        pixmap.fill(couleur);
        b->setIcon(QIcon(pixmap));
    }
}

void Palette::supprimerCouleur()
{
    treeWidget->hide();
    QList<QTreeWidgetItem *> l = treeWidget->selectedItems();
    for (int i = 0; i < l.size(); i++)
    {
        treeWidget->removeItemWidget(l[i], 1);
        treeWidget->removeItemWidget(l[i], 2);
        delete treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(l[i]));
    }
    treeWidget->show();
    boutonOk->setDisabled(!treeWidget->topLevelItemCount());
}

void Palette::genererCouleurs()
{
    bool ok;
    int nb = QInputDialog::getInt(this, tr("Génération aléatoire des couleurs"), tr("Combien de couleurs doivent être générées ?"), 100, 1, 2147483647, 1, &ok);

    if (!ok)
        return;

    QList<Couleur> l;
    QList<QSpinBox *> lBis;

    Couleur c;
    c.nombre = 1;

    treeWidget->clear();
    treeWidget->hide();

    for (int i = 0; i < nb; i++)
    {
        c.couleur = QColor(rand()%256, rand()%256, rand()%256);
        if (m_editionCanalAlpha)
            c.couleur.setAlpha(rand()%256);
        if (l.contains(c))
            lBis[++l[l.indexOf(c)].nombre];
        else
        {
            l << c;
            lBis << creerItem(l[i]);
        }
    }

    treeWidget->show();
}

QSpinBox *Palette::creerItem(const Couleur &c, int index)
{
    if (index == -1)
        index = treeWidget->topLevelItemCount();
    QTreeWidgetItem *item = new QTreeWidgetItem;
    treeWidget->insertTopLevelItem(index, item);
    QPushButton *bouton = new QPushButton;
    bouton->setIconSize(QSize(64, 16));
    QPixmap pixmap(64, 16);
    pixmap.fill(c.couleur);
    bouton->setIcon(QIcon(pixmap));
    QPalette p = bouton->palette();
    p.setColor(QPalette::Button, c.couleur);
    bouton->setPalette(p);
    connect(bouton, SIGNAL(clicked()), this, SLOT(modifierCouleur()));
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setRange(1, QWIDGETSIZE_MAX);
    spinBox->setValue(c.nombre);
    item->setData(0, Qt::DisplayRole, tr("#")+QString::number(m_editionCanalAlpha ? c.couleur.rgba() : c.couleur.rgb(), 16));
    treeWidget->setItemWidget(item, 1, bouton);
    treeWidget->setItemWidget(item, 2, spinBox);
    boutonOk->setDisabled(false);

    return spinBox;
}

void Palette::ok()
{
    listeCouleurs.clear();

    Couleur c;

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        if (m_editionCanalAlpha)
            c.couleur = QColor::fromRgba(treeWidget->topLevelItem(i)->data(0, Qt::DisplayRole).toString().remove("#").toUInt(0, 16));
        else
            c.couleur = QColor::fromRgb(treeWidget->topLevelItem(i)->data(0, Qt::DisplayRole).toString().remove("#").toUInt(0, 16));
        c.nombre = qobject_cast<QSpinBox *>(treeWidget->itemWidget(treeWidget->topLevelItem(i), 2))->value();
        if (listeCouleurs.contains(c))
            listeCouleurs[listeCouleurs.indexOf(c)].nombre += c.nombre;
        else
            listeCouleurs << c;
    }

    accept();
}

void Palette::annuler()
{
    close();

    treeWidget->clear();

    if (rafraichirEnFermant)
    {
        treeWidget->hide();

        for (int i = 0; i < listeCouleurs.size(); i++)
            creerItem(listeCouleurs[i]);

        treeWidget->show();
    }

    reject();
}

bool Palette::getRafraichirEnFermant() const
{
    return rafraichirEnFermant;
}

void Palette::setRafraichirEnFermant(bool b)
{
    rafraichirEnFermant = b;
}
