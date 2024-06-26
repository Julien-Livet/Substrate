#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <QList>
#include <QPointF>
#include <QWidget>
#include <QLineEdit>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

class Interpolation;

class BoiteCoordonnees : public QWidget
{
    Q_OBJECT

    public:
        BoiteCoordonnees(double x, double y, QwtPlot *p);
        double getX() const;
        double getY() const;
        int exec();

    private:
        QLineEdit *lineEditX;
        QLineEdit *lineEditY;
        int r;

    private slots:
        void accept();
        void reject();

    protected:
        void closeEvent(QCloseEvent *event);
};

class CanvasPicker : public QObject
{
    Q_OBJECT

    public:
        CanvasPicker(QwtPlot *plot);
        virtual bool eventFilter(QObject *, QEvent *);
        QwtPlot *plot() const;

    private:
        QwtPlotMarker *pm;
        QPointF oldPos;
        void select(const QPoint &);
        void move(const QPoint &);
        void moveBy(int dx, int dy);
        void add(const QPoint &);
        void remove();
        void showCursor(bool enable);
        void actualiser();
};

class Graphique : public QwtPlot
{
    Q_OBJECT

    public:
        Graphique(Interpolation *i, QWidget *parent = 0);
        ~Graphique();
        QwtPlotCurve *getCourbe() const;
        Interpolation *getInterpolation() const;
        QList<QwtPlotMarker *> getListePlotMarkers() const;
        void actualiser();

    public slots:
        void changerInterpolation(int i);

    private:
        Interpolation *interpolation;
        QwtPlotCurve *courbe;
        QList<QwtPlotMarker *> listePlotMarkers;
        CanvasPicker *canvasPicker;
};

class Interpolation
{
    public:
        enum Type
        {
            Constante = 0,
            Affine = 1,
            Parabolique = 2,
            Cubique = 3,
            Exponentielle = 4,
            Logarithmique = 5,
            Hyperbole = 6,
            Puissance = 7,
            Sinus = 8,
            ConstanteParMorceaux = 9,
            AffineParMorceaux = 10,
            Polynomiale = 11
        };
        Interpolation();
        Type getType() const;
        void setType(Type t, const QList<QPointF> &l = QList<QPointF>());
        const QList<QPointF> &getListePoints() const;
        void setListePoints(QList<QPointF> l);
        bool ajouterPoint(const QPointF &p);
        bool supprimerPoint(const QPointF &p);
        double calculerImage(qreal x);
        int exec(QWidget *parent = 0);
        void enregistrer(QDataStream &out) const;
        void charger(QDataStream &in, const QString &version);

    private:
        Type type;
        QList<QPointF> listePoints;
        QList<double> listeCoefficients;
        Graphique *graphique;
        void calculerCoefficients();
};

#endif // INTERPOLATION_H
