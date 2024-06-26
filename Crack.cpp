#include <QApplication>
#include <cmath>

#include "Crack.h"
#include "Substrate.h"
#include "SandPainter.h"

#define STEP 0.42

Crack::Crack(Substrate *s) : QObject()
{
    moveToThread(s);

    substrate = s;
    sandPainter = new SandPainter(this);

    // find placement along existing crack

    dimx = substrate->getRect().width();
    dimy = substrate->getRect().height();
    cgrid = substrate->getGrilleCracks();
    x = substrate->generateur(Substrate::GenerateurCracks)() % dimx;
    y = substrate->generateur(Substrate::GenerateurCracks)() % dimy;
    t = substrate->generateur(Substrate::GenerateurCracks)() % 360;
    curved = false;
    degrees_drawn = 0;
    termine = false;
    numCracks = 0;
    penWidth = 1;

    findStart();
}

Crack::~Crack()
{
    delete sandPainter;
}

void Crack::move()
{
    if (termine)
        return;

    /* continue cracking */
    if (!curved)
    {
        x += ((double)STEP * cos(t * M_PI / 180));
        y += ((double)STEP * sin(t * M_PI / 180));
    }
    else
    {
        x += ((float)ys * cos(t * M_PI / 180));
        y += ((float)ys * sin(t * M_PI / 180));

        x += ((float)xs * cos(t * M_PI / 180 - M_PI / 2));
        y += ((float)xs * sin(t * M_PI / 180 - M_PI / 2));

        t += t_inc;
        degrees_drawn += fabs(t_inc);
    }

    /* bounds check */
    /* modification of random(-0.33,0.33) */
    std::uniform_real_distribution<> dis(-0.33, 0.33);
    int cx = (int)(x + dis(substrate->generateur(Substrate::GenerateurCracks)));
    int cy = (int)(y + dis(substrate->generateur(Substrate::GenerateurCracks)));

    if (cx >= 0 && cx < dimx && cy >= 0 && cy < dimy)
    {
        /* draw sand painter if we're not wireframe */
        if (!substrate->getFilDeFer())
            regionColor();

        /* draw fgcolor crack */
        substrate->dessinerCrack(pointPrecedent, QPoint(cx, cy), couleur, penWidth);
        pointPrecedent = QPoint(cx, cy);

        //substrate->dessinerSable(QPoint(cx, cy), couleurs.last(), penWidth);

        if (curved && degrees_drawn > 360)
        {
            if (substrate->incrementerNombreCracksFinis(this))
            {
                termine = true;
                return;
            }
            /* completed the circle, stop cracking */
            findStart(); /* restart ourselves */
            substrate->createCrack(); /* generate a new crack */
        }
        /* safe to check */
        else if (cgrid[cy * dimx + cx] > 10000 || fabs(cgrid[cy * dimx + cx] - t) < 5)
        {
            /* continue cracking */
            cgrid[cy * dimx + cx] = (int)t;
        }
        else if (fabs(cgrid[cy * dimx + cx] - t) > 2)
        {
            if (substrate->incrementerNombreCracksFinis(this))
            {
                termine = true;
                return;
            }
            /* crack encountered (not self), stop cracking */
            findStart(); /* restart ourselves */
            substrate->createCrack(); /* generate a new crack */
        }
    }
    else
    {
        if (substrate->incrementerNombreCracksFinis(this))
        {
            termine = true;
            return;
        }

        /* out of bounds, stop cracking */

        /* need these in case of timeout in start_crack */
        x = substrate->generateur(Substrate::GenerateurCracks)() % dimx;
        y = substrate->generateur(Substrate::GenerateurCracks)() % dimy;
        t = substrate->generateur(Substrate::GenerateurCracks)() % 360;

        findStart(); /* restart ourselves */
        substrate->createCrack(); /* generate a new crack */
    }
}

void Crack::findStart()
{
    // pick random point
    int px = 0;
    int py = 0;

    // shift until crack is found
    bool found = false;
    int timeout = 0;
    do
    {
        px = substrate->generateur(Substrate::GenerateurCracks)() % dimx;
        py = substrate->generateur(Substrate::GenerateurCracks)() % dimy;
        if (cgrid[py * dimx + px] < 1000)
            found = true;
    }
    while (!found && timeout++ < 1000);

    if (!found)
    {
        // We timed out.  Use our default values
        px = x;
        py = y;

        // Sanity check needed
        if (px < 0)
            px = 0;
        if (px >= dimx)
            px = dimx-1;
        if (py < 0)
            py = 0;
        if (py >= dimy)
            py = dimy - 1;

        cgrid[py * dimx + px] = t;
        found = true;
    }

    couleur = substrate->getCouleurAleatoire(Substrate::CouleurCracks);

    sandPainter->changerCouleur();

    if (found)
    {
        // start crack
        int a = cgrid[py * dimx + px];
        std::uniform_real_distribution<> dis(-2.0, 2.1);
        if (substrate->generateur(Substrate::GenerateurCracks)() % 100 < 50)
            a -= 90 + int(dis(substrate->generateur(Substrate::GenerateurCracks)));
        else
            a += 90 + int(dis(substrate->generateur(Substrate::GenerateurCracks)));

        if (substrate->generateur(Substrate::GenerateurCracks)() % 100 < substrate->getCirclePercent())
        {
            double r; /* radius */
            double radian_inc;

            curved = true;
            degrees_drawn = 0;

            r = 10 + (substrate->generateur(Substrate::GenerateurCracks)() % ((dimx + dimy) / 2));

            if (substrate->generateur(Substrate::GenerateurCracks)() % 100 < 50)
                r *= -1;

            /* arc length = r * theta => theta = arc length / r */
            radian_inc = STEP / r;
            t_inc = radian_inc * 360 / 2 / M_PI;

            ys = r * sin(radian_inc);
            xs = r * (1 - cos(radian_inc));
        }
        else
            curved = false;

        pointPrecedent = QPoint(px, py);
        startCrack(px, py, a);
        penWidth = substrate->getPenWidth();

        numCracks++;

        substrate->incrementerNombreCracksDebutes();
    }
    else
    {
        //println("timeout: "+timeout);
        emit ended();
    }
}

void Crack::startCrack(int X, int Y, int T)
{
    x = X;
    y = Y;
    t = T;//%360;
    x += 0.61 * cos(t * M_PI / 180.0);
    y += 0.61 * sin(t * M_PI / 180.0);
}

void Crack::regionColor()
{
    // start checking one step away
    double rx = x;
    double ry = y;
    bool openspace = true;

    // find extents of open space
    while (openspace)
    {
      // move perpendicular to crack
      rx += 0.81 * sin(t * M_PI / 180.0);
      ry -= 0.81 * cos(t * M_PI / 180.0);
      int cx = int(rx);
      int cy = int(ry);
      if (cx >= 0 && cx < dimx && cy >= 0 && cy < dimy)
      {
        // safe to check
        if (cgrid[cy * dimx + cx] <= 10000) // space is not open
            openspace = false;
      }
      else
          openspace = false;
    }
    // draw sand painter
    sandPainter->render(rx, ry, x, y);
}

Substrate * Crack::getSubstrate() const
{
    return substrate;
}

SandPainter * Crack::getSandPainter() const
{
    return sandPainter;
}

bool Crack::getTermine() const
{
    return termine;
}

int Crack::getNumCracks() const
{
    return numCracks;
}

qreal Crack::getPenWidth() const
{
    return penWidth;
}

int Crack::getX() const
{
    return x;
}

int Crack::getY() const
{
    return y;
}

int Crack::getT() const
{
    return t;
}
