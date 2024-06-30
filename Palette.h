#ifndef PALETTE_H
#define PALETTE_H

#include <QColor>
#include <QDataStream>
#include <QDialog>
#include <QPushButton>
#include <QSpinBox>
#include <QTreeWidget>

struct Couleur
{
    QColor couleur;
    int nombre;
};

QDataStream &operator<<(QDataStream &out, const Couleur &couleur);
QDataStream &operator>>(QDataStream &in, Couleur &couleur);
bool operator==(const Couleur &c1, const Couleur &c2);

class WidgetPalette : public QWidget
{
    public:
        WidgetPalette(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
        const QList<Couleur> &getListeCouleurs() const;
        void setListeCouleurs(const QList<QColor> &liste);
        void setListeCouleurs(const QList<Couleur> &liste);
        void setListeCouleurs(const QStringList &liste);
        int longueurOptimale() const;
        void genererCouleurs(bool editionCanalAlpha = false);

    private:
        QList<Couleur> listeCouleurs;

    protected:
        void paintEvent(QPaintEvent *event);
};

class Palette : public QDialog
{
    Q_OBJECT

    public:
        Palette(bool editionCanalAlpha = false, QWidget *parent = 0);
        const QList<Couleur> &getListeCouleurs() const;
        void setListeCouleurs(const QList<Couleur> &liste);
        bool getRafraichirEnFermant() const;
        void setRafraichirEnFermant(bool b);

    private:
        QList<Couleur> listeCouleurs;
        QTreeWidget *treeWidget;
        QPushButton *boutonAjouter;
        QPushButton *boutonInserer;
        QPushButton *boutonModifier;
        QPushButton *boutonSupprimer;
        QPushButton *boutonGenerer;
        QPushButton *boutonOk;
        bool m_editionCanalAlpha;
        bool rafraichirEnFermant;
        QSpinBox *creerItem(const Couleur &c, int index = -1);

    private slots:
        void ajouterCouleur();
        void insererCouleur();
        void supprimerCouleur();
        void genererCouleurs();
        void modifierCouleur();
        void ok();
        void annuler();
};

#endif // PALETTE_H
