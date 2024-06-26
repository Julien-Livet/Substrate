#include <cmath>

#include "SandPainter.h"
#include "Crack.h"
#include "Substrate.h"

SandPainter::SandPainter(Crack *cr)
{
    crack = cr;
    sandp = 0;
    grains = crack->getSubstrate()->getGrains();
}

void SandPainter::render(double x, double y, double ox, double oy)
{
    /* Modulate gain */
    std::uniform_real_distribution<> dis(-0.050, 0.050);
    sandg += dis(crack->getSubstrate()->generateur(Substrate::GenerateurSable));
    double maxg = 1.0;

    if (sandg < 0)
        sandg = 0;

    if (sandg > maxg)
        sandg = maxg;

    /* Lay down grains of sand */
    double w = sandg/(grains-1);

    for (int i = 0; i < grains; i++)
    {
        double a = 0.1-i/(grains*10.0);
        sandcolor.setAlpha(a*255);
        crack->getSubstrate()->dessinerSable(QPointF(ox+(x-ox)*sin(sandp+sin(i*w)), oy+(y-oy)*sin(sandp+sin(i*w))), sandcolor);
    }
}

Crack * SandPainter::getCrack() const
{
    return crack;
}

void SandPainter::changerCouleur()
{
    std::uniform_real_distribution<> dis(-0.01, 0.1);
    sandg = dis(crack->getSubstrate()->generateur(Substrate::GenerateurSable));
    sandcolor = crack->getSubstrate()->getCouleurAleatoire(Substrate::CouleurSable);
}
