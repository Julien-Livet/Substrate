#ifndef SUBSTRATE_H
#define SUBSTRATE_H

#include <random>

#include <QObject>
#include <QTimer>
#include <QColor>
#include <QImage>
#include <QStringList>
#include <QPen>
#include <QPainter>
#include <QPicture>
#include <QThread>

#include "Crack.h"
#include "Interpolation.h"
#include "Palette.h"
#include "Degrade.h"
#include "constantes.h"

class Substrate : public QThread
{
    Q_OBJECT

    public:
        enum TypeGenerateur
        {
            GenerateurCracks = 0,
            GenerateurSable = 1,
            GenerateurCouleurCracks = 2,
            GenerateurCouleurSable = 3,
            GenerateurCouleurZones = 4,
            GenerateurCouleurFond = 5,
            GenerateurAlphaZones = 6
        };
        enum TypeCouleur
        {
            CouleurCracks = 0,
            CouleurSable = 1,
            CouleurZones = 2,
            CouleurFond = 3
        };
        Substrate(QObject *parent = 0);
        Substrate(const QRect &r, QObject *parent = 0);
        ~Substrate();
        const QRect &getRect() const;
        void setRect(const QRect &r);
        int *getGrilleCracks() const;
        bool getDemarre() const;
        int getPeriodeCroissance() const;
        bool getFilDeFer() const;
        void setFilDeFer(bool b);
        int getNombreCracksFinal() const;
        void setNombreCracksFinal(int nb);
        int getNombreCracksInitial() const;
        void setNombreCracksInitial(int nb);
        const QList<Crack *> &getListeCracks() const;
        int getGrains() const;
        void setGrains(int g);
        int getCirclePercent() const;
        void setCirclePercent(int p);
        int getMaxCouleurs(TypeCouleur type) const;
        void setCouleurs(TypeCouleur type, const QList<Couleur> &l, int maxCouleurs = 0);//0 pour toute les couleurs fournies
        void setCouleurs(TypeCouleur type, const QList<QColor> &l, int maxCouleurs = 0);//0 pour toute les couleurs fournies
        void setCouleurs(TypeCouleur type, const QStringList &l, int maxCouleurs = 0);//0 pour toute les couleurs fournies
        void setCouleurs(TypeCouleur type, const Degrade &degrade, int maxCouleurs = 0);//0 pour toute les couleurs fournies
        void setCouleurs(TypeCouleur type, const QString &fichier, int maxCouleurs = 0);//0 pour toute les couleurs fournies
        void setCouleurs(TypeCouleur type, const QImage &image, int maxCouleurs = 0);//0 pour toute les couleurs fournies
        bool getLimiteCracksAtteinte();
        int getChargement() const;
        qreal getPenWidth() const;
        qreal getFacteurEchelle() const;
        void setFacteurEchelle(qreal facteur);
        const Interpolation &getInterpolation() const;
        void setInterpolation(const Interpolation &i);
        bool getRemplissageZones() const;
        void setRemplissageZones(bool b);
        bool getAlphaZonesFixee() const;
        void setAlphaZonesFixee(bool b);
        int getValeurAlphaZonesFixee() const;
        void setValeurAlphaZonesFixee(int a);
        bool getEstTermine() const;
        void generer(QPaintDevice *device, const QPointF &offset = QPointF(), uint numero = 1, int periode = -1);
        bool getGenerationCadencee() const;
        void setPixmapFond(const QPixmap &pixmap);
        const QPixmap &getPixmapFond() const;
        std::default_random_engine &generateur(TypeGenerateur type);
        quint64 getCleGenerateur(TypeGenerateur type) const;
        void setCleGenerateur(TypeGenerateur type, quint64 cle);
        bool getGenerateurForce(TypeGenerateur type) const;
        void setGenerateurForce(TypeGenerateur type, bool b);
        const QList<QColor> &getCouleurs(TypeCouleur type) const;
        const QColor &getCouleurAleatoire(TypeCouleur type);
        uint getNumeroSubstrate() const;
        void initialiserGenerateurs();
        QThread::Priority getPrioriteThread() const;
        void setPrioriteThread(QThread::Priority priorite);
        bool getAntialiasingActive() const;
        void setAntialiasingActive(bool b);
        int getProbabiliteDeboublementCrack() const;
        void setProbabiliteDeboublementCrack(int probabilite);
        int getNombreCracksSimultanesEnCreation() const;
        void setNombreCracksSimultanesEnCreation(int nb);
        bool getRepassageFinalCracks() const;
        void setRepassageFinalCracks(bool b);

    public slots:
        void arreter();

    signals:
        void termine();
        void arrete();
        void chargementChange(int pourcentage);
        void modifie();
        void debutRemplissageZones();

    private:
        QList<Crack *> listeCracks;
        int *grilleCracks;
        QRect rect;
        int periodeCroissance;
        int nombreCracksFinal;
        int nombreCracksInitial;
        bool filDeFer;
        int grains;
        int circle_percent;
        int maxCouleurs;
        int chargement;
        bool arret;
        qreal facteurEchelle;
        Interpolation interpolation;
        bool remplissageZones;
        bool alphaZonesFixee;
        int valeurAlphaZonesFixee;
        int nombreCracksDebutes;
        int nombreCracksFinis;
        bool demarre;
        QPixmap pixmapFond;
        qreal m_penWidth;
        QPainter *painterSubstrate;
        std::default_random_engine generateurs[NOMBRE_GENERATEURS];
        qint64 clesGenerateurs[NOMBRE_GENERATEURS];
        bool generateursForces[NOMBRE_GENERATEURS];
        QList<QColor> couleurs[NOMBRE_COULEURS];
        int maximaCouleurs[NOMBRE_COULEURS];
        bool generationCadencee;
        uint numeroSubstrate;
        bool boucleTerminee;
        bool estTermine;
        QImage *imageCracks;
        QPainter *painterImage;
        QThread::Priority prioriteThread;
        bool antialiasingActive;
        int probabiliteDedoublementCrack;
        int nombreCracksSimultanesEnCreation;
        bool repassageFinalCracks;
        void initialiserGeneration(QPaintDevice *device, const QPointF &offset);
        void createCrack();
        void makeCrack();
        void calculerCoefficientsInterpolation();
        void remplirZones();
        bool remplirZone(int x, int y);
        void dessinerSable(const QPointF &p, const QColor &c);
        void dessinerCrack(const QPoint &p1, const QPoint &p2, const QColor &c, qreal penWidth = 1);
        void initialisation();
        void nettoyer();
        void incrementerNombreCracksDebutes();
        bool incrementerNombreCracksFinis(Crack *crack);
        friend class Crack;
        friend class SandPainter;

    protected:
        void run();
};

#endif // SUBSTRATE_H
