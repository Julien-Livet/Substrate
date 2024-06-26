#ifndef DEGRADE_H
#define DEGRADE_H

#include <QWidget>
#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>

struct Degrade
{
    QColor couleurDepart;
    QList<QPair<int, QColor> > listeCouleurs;
};

QDataStream &operator<<(QDataStream &out, const Degrade &degrade);
QDataStream &operator>>(QDataStream &in, Degrade &degrade);
bool operator==(const Degrade &d1, const Degrade &d2);

class WidgetDegrade : public QWidget
{
    public:
        WidgetDegrade(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
        const Degrade &getDegrade() const;
        void setDegrade(const Degrade &d);
        int longueurOptimale() const;
        void genererCouleurs(bool editionCanalAlpha = false);

    private:
        Degrade degrade;

    protected:
        void paintEvent(QPaintEvent *event);
};

class DialogueDegrade : public QDialog
{
    Q_OBJECT

    public:
        DialogueDegrade(bool editionCanalAlpha = false, QWidget *parent = 0);
        const Degrade &getDegrade() const;
        void setDegrade(const Degrade &d);
        bool getRafraichirEnFermant() const;
        void setRafraichirEnFermant(bool b);

    private:
        Degrade degrade;
        QTreeWidget *treeWidget;
        QPushButton *boutonAjouter;
        QPushButton *boutonInserer;
        QPushButton *boutonModifier;
        QPushButton *boutonSupprimer;
        QPushButton *boutonGenerer;
        QPushButton *boutonCouleurDepart;
        QPushButton *boutonOk;
        bool m_editionCanalAlpha;
        bool rafraichirEnFermant;
        void creerItem(const QPair<int, QColor> &pair, int index = -1);

    private slots:
        void ajouterCouleur();
        void insererCouleur();
        void supprimerCouleur();
        void genererCouleurs();
        void modifierCouleur();
        void ok();
        void annuler();
        void modifierCouleurDepart();
};

#endif // DEGRADE_H
