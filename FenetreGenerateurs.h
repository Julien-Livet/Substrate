#ifndef FENETREGENERATEURS_H
#define FENETREGENERATEURS_H

#include <QWidget>
#include <QTableWidget>
#include <QCheckBox>
#include <QPushButton>

class Substrate;

class FenetreGenerateurs : public QWidget
{
    Q_OBJECT

    public:
        FenetreGenerateurs(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
        void chargerSubstrate(Substrate *s) const;
        void chargerClesSubstrate(Substrate *s);
        void enregistrer(QDataStream &out) const;
        void charger(QDataStream &in, const QString &version);

    private:
        QTableWidget *tableWidget;
        QList<bool> listeForcages;
        QStringList listeCles;
        QCheckBox *checkBoxForcage;
        QPushButton *boutonGenerer;
        void enregistrerEtat();

    private slots:
        void genererCleAleatoire();
        void ok();
        void annuler();
        void forcageChange(int etat);
        void forcerGenerateurs(int etat);
        void genererClesAleatoires();

    protected:
        void showEvent(QShowEvent *event);
        void closeEvent(QCloseEvent *event);
};

#endif // FENETREGENERATEURS_H
