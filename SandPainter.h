#ifndef SANDPAINTER_H
#define SANDPAINTER_H

#include <QColor>

class Crack;

class SandPainter
{
    public:
        SandPainter(Crack *cr);
        void initialiser();
        void render(double x, double y, double ox, double oy);
        Crack *getCrack() const;
        void changerCouleur();

    private:
        QColor sandcolor;
        double sandp;
        double sandg;
        double degrees_drawn;
        Crack *crack;
        int grains;
};

#endif // SANDPAINTER_H
