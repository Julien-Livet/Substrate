#ifndef CRACK_H
#define CRACK_H

#include <QColor>
#include <QObject>
#include <QPointF>

class Substrate;
class SandPainter;

class Crack : public QObject
{
    Q_OBJECT

    public:
        Crack(Substrate *s);
        ~Crack();
        void move();
        Substrate *getSubstrate() const;
        SandPainter *getSandPainter() const;
        bool getTermine() const;
        int getNumCracks() const;
        qreal getPenWidth() const;
        int getX() const;
        int getY() const;
        int getT() const;

    signals:
        void ended();

    private:
        Substrate *substrate;
        double x, y;
        double t;    // direction of travel in degrees
        double ys, xs, t_inc;
        bool curved;
        double degrees_drawn;
        SandPainter *sandPainter;
        bool termine;
        int numCracks;
        qreal penWidth;/*
        QPoint point1;
        QPoint point2;
        QPoint point3;*/
        QPoint pointPrecedent;
        QColor couleur;
        int dimx;
        int dimy;
        int *cgrid;
        void findStart();
        void startCrack(int X, int Y, int T);
        void regionColor();
        friend class Substrate;
};

#endif // CRACK_H
