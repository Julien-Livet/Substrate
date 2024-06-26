#include <QTimer>
#include <QApplication>
#include <QPainterPath>
#include <QRadialGradient>
#include <ctime>
#include <cmath>

#include "Substrate.h"

QStringList listeCouleursDefaut = QStringList() <<
    "#201f21" << "#262c2e" << "#352626" << "#372b27" <<
    "#302c2e" << "#392b2d" << "#323229" << "#3f3229" <<
    "#38322e" << "#2e333d" << "#333a3d" << "#473329" <<
    "#40392c" << "#40392e" << "#47402c" << "#47402e" <<
    "#4e402c" << "#4f402e" << "#4e4738" << "#584037" <<
    "#65472d" << "#6d5d3d" << "#745530" << "#755532" <<
    "#745d32" << "#746433" << "#7c6c36" << "#523152" <<
    "#444842" << "#4c5647" << "#655d45" << "#6d5d44" <<
    "#6c5d4e" << "#746c43" << "#7c6c42" << "#7c6c4b" <<
    "#6b734b" << "#73734b" << "#7b7b4a" << "#6b6c55" <<
    "#696d5e" << "#7b6c5d" << "#6b7353" << "#6a745d" <<
    "#727b52" << "#7b7b52" << "#57746e" << "#687466" <<
    "#9c542b" << "#9d5432" << "#9d5b35" << "#936b36" <<
    "#aa7330" << "#c45a27" << "#d95223" << "#d85a20" <<
    "#db5a23" << "#e57037" << "#836c4b" << "#8c6b4b" <<
    "#82735c" << "#937352" << "#817b63" << "#817b6d" <<
    "#927b63" << "#d9893b" << "#e49832" << "#dfa133" <<
    "#e5a037" << "#f0ab3b" << "#8a8a59" << "#b29a58" <<
    "#89826b" << "#9a8262" << "#888b7c" << "#909a7a" <<
    "#a28262" << "#a18a69" << "#a99968" << "#99a160" <<
    "#99a168" << "#ca8148" << "#eb8d43" << "#c29160" <<
    "#c29168" << "#d1a977" << "#c9b97f" << "#f0e27b" <<
    "#9f928b" << "#c0b999" << "#e6b88f" << "#c8c187" <<
    "#e0c886" << "#f2cc85" << "#f5da83" << "#ecde9d" <<
    "#f5d294" << "#f5da94" << "#f4e784" << "#f4e18a" <<
    "#f4e193" << "#e7d8a7" << "#f1d4a5" << "#f1dca5" <<
    "#f4dbad" << "#f1dcae" << "#f4dbb5" << "#f5dbbd" <<
    "#f4e2ad" << "#f5e9ad" << "#f4e3be" << "#f5eabe" <<
    "#f7f0b6" << "#d9d1c1" << "#e0d0c0" << "#e7d8c0" <<
    "#f1ddc6" << "#e8e1c0" << "#f3edc7" << "#f6ecce" <<
    "#f8f2c7" << "#efefd0";

Substrate::Substrate(QObject *parent) : QThread(parent)
{
    initialisation();
}

Substrate::Substrate(const QRect &r, QObject *parent) : QThread(parent)
{
    initialisation();
    setRect(r);
}

void Substrate::initialisation()
{
    grilleCracks = 0;
    couleurs[int(CouleurFond)] << Qt::white;
    couleurs[int(CouleurCracks)] << Qt::black;
    nombreCracksInitial = 3;
    nombreCracksFinal = 500;
    filDeFer = false;
    grains = 64;
    circle_percent = 33;
    maxCouleurs = 100;
    chargement = 0;
    facteurEchelle = 1;
    remplissageZones = false;
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
        generateursForces[i] = false;
    for (int i = 0; i < NOMBRE_COULEURS; i++)
        maximaCouleurs[i] = 100;
    for (int i = 2; i < NOMBRE_COULEURS; i++)
        setCouleurs(TypeCouleur(i), listeCouleursDefaut, 100);
        //setCouleurs(TypeCouleur(i), QString("pollockShimmering.gif"), 100);
    painterSubstrate = 0;
    nombreCracksDebutes = 0;
    nombreCracksFinis = 0;
    arret = false;
    demarre = false;
    valeurAlphaZonesFixee = 255;
    alphaZonesFixee = true;
    boucleTerminee = true;
    estTermine = false;
    imageCracks = 0;
    painterImage = 0;
    prioriteThread = QThread::InheritPriority;
    antialiasingActive = true;
    periodeCroissance = -1;
    probabiliteDedoublementCrack = 75;
    nombreCracksSimultanesEnCreation = sqrt(nombreCracksFinal);
    repassageFinalCracks = true;
}

Substrate::~Substrate()
{
    if (!arret && demarre && !estTermine)
        arreter();
    if (!boucleTerminee)
        if (!wait())
            terminate();
    nettoyer();
}

const QRect & Substrate::getRect() const
{
   return rect;
}

void Substrate::setRect(const QRect &r)
{
    rect = r;
}

int * Substrate::getGrilleCracks() const
{
    return grilleCracks;
}

bool Substrate::getDemarre() const
{
    return demarre;
}

int Substrate::getPeriodeCroissance() const
{
    return periodeCroissance;
}

bool Substrate::getFilDeFer() const
{
    return filDeFer;
}

void Substrate::setFilDeFer(bool b)
{
    filDeFer = b;
}

int Substrate::getNombreCracksFinal() const
{
    return nombreCracksFinal;
}

void Substrate::setNombreCracksFinal(int nb)
{
    nombreCracksFinal = nb;
}

int Substrate::getNombreCracksInitial() const
{
    return nombreCracksInitial;
}

void Substrate::setNombreCracksInitial(int nb)
{
    nombreCracksInitial = nb;
}

void Substrate::arreter()
{
    if (arret || !demarre || estTermine)
        return;
    arret = true;
}

void Substrate::generer(QPaintDevice *device, const QPointF &offset, uint numero, int periode)
{
    generationCadencee = (periode >= 0);

    numeroSubstrate = numero;
    periodeCroissance = periode;

    initialiserGeneration(device, offset);
}

void Substrate::initialiserGeneration(QPaintDevice *device, const QPointF &offset)
{
    nettoyer();

    grilleCracks = new int[rect.width()*rect.height()];

    for (int y = 0; y < rect.height(); y++)
        for (int x = 0; x < rect.width(); x++)
            grilleCracks[y*rect.width()+x] = 10001;

    painterSubstrate = new QPainter(device);
    if (antialiasingActive)
        painterSubstrate->setRenderHints(QPainter::Antialiasing);
    if ((!filDeFer || remplissageZones) && repassageFinalCracks)
    {
        imageCracks = new QImage(rect.width()*facteurEchelle, rect.height()*facteurEchelle, QImage::Format_ARGB32);
        imageCracks->fill(QColor(0, 0, 0, 0));
        painterImage = new QPainter(imageCracks);
    }

    nombreCracksDebutes = 0;
    nombreCracksFinis = 0;
    chargement = 0;
    arret = false;
    demarre = true;
    estTermine = false;
    boucleTerminee = true;
    m_penWidth = interpolation.calculerImage(0);
    if (m_penWidth <= 0)
        m_penWidth = 1;

    QList<int> liste;

    // make random crack seeds
    for (int k = 0; k < nombreCracksInitial; k++)//16
    {
        int i = (generateurs[int(GenerateurCracks)]() % rect.height()) * rect.width()
                + generateurs[int(GenerateurCracks)]() % rect.width();
        liste << i;
        grilleCracks[i] = generateurs[int(GenerateurCracks)]() % 360;
    }

    painterSubstrate->translate(offset);
    painterSubstrate->translate(rect.topLeft());

    if (!pixmapFond.isNull())
        painterSubstrate->drawPixmap(0, 0, pixmapFond);
    painterSubstrate->fillRect(QRectF(QPointF(0, 0), rect.size() * facteurEchelle), getCouleurAleatoire(CouleurFond));

    // make just three cracks
    for (int k = 0; k < nombreCracksInitial; k++)
       makeCrack();

    for (int i = 0; i < liste.size(); i++)
    {
        bool b = true;
        for (int j = 0; j < listeCracks.size(); j++)
        {
            if (liste[i] == listeCracks[j]->getY()* rect.width() + listeCracks[j]->getX())
            {
                b = false;
                break;
            }
        }
        if (b)
            grilleCracks[liste[i]] = 10001;
    }
    liste.clear();

    start(prioriteThread);
}

void Substrate::makeCrack()
{
    if (nombreCracksDebutes < nombreCracksSimultanesEnCreation && nombreCracksDebutes < nombreCracksFinal)
    {
        // make a new crack instance
        listeCracks << new Crack(this);
    }
}

void Substrate::createCrack()
{
    if (generateurs[int(GenerateurCracks)]() % 100 + 1 < probabiliteDedoublementCrack)
        makeCrack();
}

const QList<Crack *> & Substrate::getListeCracks() const
{
    return listeCracks;
}

void Substrate::dessinerSable(const QPointF &p, const QColor &c)
{
    if (arret || !c.alpha() || (remplissageZones && alphaZonesFixee && valeurAlphaZonesFixee == 255))
        return;

    QPointF point = p*facteurEchelle;

    painterSubstrate->setPen(QPen(c, 1));

    if (facteurEchelle == 1)
        painterSubstrate->drawPoint(point);
    else
    {
        const double r = facteurEchelle;
        QRadialGradient rg(point, r, point);
        rg.setColorAt(0, c);
        QColor c1 = c;
        c1.setAlpha(0);
        rg.setColorAt(1, c1);
        QPainterPath path;
        path.addEllipse(point, r, r);
        painterSubstrate->fillPath(path, rg);
        //painterSubstrate->fillPath(path, c);
    }
}

void Substrate::dessinerCrack(const QPoint &p1, const QPoint &p2, const QColor &c, qreal penWidth)
{
    if (arret || !c.alpha())
        return;

    if (penWidth <= 0)
        penWidth = 1;
    penWidth *= facteurEchelle;

    painterSubstrate->setPen(QPen(c, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painterSubstrate->drawLine(p1 * facteurEchelle, p2 * facteurEchelle);

    if (painterImage)
    {
        painterImage->setPen(QPen(c, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painterImage->drawLine(p1 * facteurEchelle, p2 * facteurEchelle);
    }

    if (fabs(p2.x() - p1.x()) > 1)
    {
        double a = tan(-QLineF(p1, p2).angle() * M_PI / 180);
        double b = (double)p1.y() - a * (double)p1.x();
        int dx = p2.x() - p1.x();
        dx /= fabs(dx);
        for (int x = p1.x() + dx; x != p2.x() - dx; x += dx)
        {
            double y = a * (double)x + b;
            grilleCracks[(int)y * rect.width() + x] = grilleCracks[p1.y() * rect.width() + p1.x()];
        }
    }
    else if (fabs(p2.y() - p1.y()) > 1)
    {
        int dy = p2.y() - p1.y();
        dy /= fabs(dy);
        for (int y = p1.y() + dy; y != p2.y() - dy; y += dy)
            grilleCracks[y * rect.width() + p1.x()] = grilleCracks[p1.y() * rect.width() + p1.x()];
    }
}

int Substrate::getGrains() const
{
    return grains;
}

void Substrate::setGrains(int g)
{
    grains = g;
}

int Substrate::getCirclePercent() const
{
    return circle_percent;
}

void Substrate::setCirclePercent(int p)
{
    circle_percent = p;
}

int Substrate::getMaxCouleurs(TypeCouleur type) const
{
    return maximaCouleurs[int(type)];
}

void Substrate::setCouleurs(TypeCouleur type, const QList<Couleur> &l, int maxCouleurs)
{
    QList<QColor> lBis;

    for (int i = 0; i < l.size(); i++)
        for (int j = 0; j < l[i].nombre; j++)
            lBis << l[i].couleur;

    setCouleurs(type, lBis, maxCouleurs);
}

void Substrate::setCouleurs(TypeCouleur type, const QList<QColor> &l, int maxCouleurs)
{
    couleurs[int(type)].clear();

    if (maxCouleurs < 0)
        maxCouleurs = 0;

    maximaCouleurs[int(type)] = maxCouleurs;

    if (l.isEmpty())
        couleurs[int(type)] << QColor(generateurs[2 + int(type)]() % 256,
                                      generateurs[2 + int(type)]() % 256,
                                      generateurs[2 + int(type)]() % 256);
    else if (!maxCouleurs || l.size() <= maxCouleurs)
        for (int i = 0; i < l.size(); i++)
            couleurs[int(type)] << l[i];
    else
        while (couleurs[int(type)].size() < maxCouleurs)
            couleurs[int(type)] << l[generateurs[2 + int(type)]() % l.size()];
}

void Substrate::setCouleurs(TypeCouleur type, const QStringList &l, int maxCouleurs)
{
    couleurs[int(type)].clear();

    if (maxCouleurs < 0)
        maxCouleurs = 0;

    maximaCouleurs[int(type)] = maxCouleurs;

    if (l.isEmpty())
        setCouleurs(type, QList<QColor>(), maxCouleurs);
    else if (!maxCouleurs || l.size() <= maxCouleurs)
    {
        bool ok;
        for (int i = 0; i < l.size(); i++)
        {
            QColor c = QColor::fromRgb(QString(l[i]).remove("#").toUInt(&ok, 16));
            if (ok)
                couleurs[int(type)] << c;
        }
    }
    else
    {
        bool ok;
        while (couleurs[int(type)].size() < maxCouleurs)
        {
            QColor c = QColor::fromRgb(QString(l[generateurs[2 + int(type)]() % l.size()]).remove("#").toUInt(&ok, 16));
            if (ok)
                couleurs[int(type)] << c;
        }
    }

    if (couleurs[int(type)].isEmpty())
        setCouleurs(type, QList<QColor>(), maxCouleurs);
}

void Substrate::setCouleurs(TypeCouleur type, const Degrade &degrade, int maxCouleurs)
{
    int x = 0;

    for (int i = 0; i < degrade.listeCouleurs.size(); i++)
        x += degrade.listeCouleurs[i].first;

    QImage image(QSize(x, 1), QImage::Format_ARGB32);

    x = 0;

    QPainter painter(&image);

    QLinearGradient linearGradient;
    linearGradient.setColorAt(0, degrade.couleurDepart);

    for (int i = 0; i < degrade.listeCouleurs.size(); i++)
    {
        linearGradient.setStart(x, 0);
        linearGradient.setColorAt(1, degrade.listeCouleurs[i].second);
        linearGradient.setFinalStop(x+degrade.listeCouleurs[i].first, 0);
        painter.fillRect(x, 0, degrade.listeCouleurs[i].first, image.height(), linearGradient);
        x += degrade.listeCouleurs[i].first;
        linearGradient.setColorAt(0, degrade.listeCouleurs[i].second);
    }

    setCouleurs(type, image, maxCouleurs);
}

void Substrate::setCouleurs(TypeCouleur type, const QString &fichier, int maxCouleurs)
{
    setCouleurs(type, QImage(fichier), maxCouleurs);
}

void Substrate::setCouleurs(TypeCouleur type, const QImage &image, int maxCouleurs)
{
    couleurs[int(type)].clear();

    if (maxCouleurs < 0)
        maxCouleurs = 0;

    maximaCouleurs[int(type)] = maxCouleurs;

    if (image.isNull())
        setCouleurs(type, QList<QColor>(), maxCouleurs);
    else if (!maxCouleurs || image.width() * image.height() <= maxCouleurs)
        for (int y = 0; y < image.height(); y++)
            for (int x = 0; x < image.width(); x++)
                couleurs[int(type)] << image.pixel(x, y);
    else
        while (couleurs[int(type)].size() < maxCouleurs)
            couleurs[int(type)] << image.pixel(generateurs[2 + int(type)]() % image.width(),
                                               generateurs[2 + int(type)]() % image.height());

    if (couleurs[int(type)].isEmpty())
        setCouleurs(type, QList<QColor>(), maxCouleurs);
}

bool Substrate::getLimiteCracksAtteinte()
{
    return (nombreCracksDebutes == nombreCracksFinal);
}

qreal Substrate::getPenWidth() const
{
    return m_penWidth;
}

qreal Substrate::getFacteurEchelle() const
{
    return facteurEchelle;
}

void Substrate::setFacteurEchelle(qreal facteur)
{
    if (facteur < 1 || facteurEchelle == facteur)
        return;

    facteurEchelle = facteur;
}

const Interpolation & Substrate::getInterpolation() const
{
    return interpolation;
}

void Substrate::setInterpolation(const Interpolation &i)
{
    interpolation = i;
}

bool Substrate::getRemplissageZones() const
{
    return remplissageZones;
}

void Substrate::setRemplissageZones(bool b)
{
    remplissageZones = b;
}

bool Substrate::getAlphaZonesFixee() const
{
    return alphaZonesFixee;
}

void Substrate::setAlphaZonesFixee(bool b)
{
    alphaZonesFixee = b;
}

int Substrate::getValeurAlphaZonesFixee() const
{
    return valeurAlphaZonesFixee;
}

void Substrate::setValeurAlphaZonesFixee(int a)
{
    valeurAlphaZonesFixee = a;
}

void Substrate::remplirZones()
{
    emit debutRemplissageZones();

    for (int y = 0; y < imageCracks->height(); y++)
    {
        for (int x = 0; x < imageCracks->width(); x++)
        {
            if (arret)
                return;
            if (remplirZone(x, y) && generationCadencee)
                emit modifie();
        }
    }
}

bool Substrate::remplirZone(int x, int y)
{
    if (imageCracks->pixel(x, y) != QColor(0, 0, 0, 0).rgba())
        return false;

    QList<QPoint> listePoints;
    listePoints << QPoint(x, y);
    QPoint p;

    QColor c = getCouleurAleatoire(CouleurZones);
    if (alphaZonesFixee)
        c.setAlpha(valeurAlphaZonesFixee);
    //else
        //c.setAlpha(generateurs[int(GenerateurAlphaZones)]%256);

    while (!arret && !listePoints.isEmpty())
    {
        p = listePoints.takeLast();

        if (imageCracks->pixel(p) == QColor(0, 0, 0, 0).rgba())
        {
            imageCracks->setPixel(p, c.rgba());

            if (p.x() + 1 < imageCracks->width())
                listePoints << QPoint(p.x() + 1, p.y());
            if (p.x() - 1 >= 0)
                listePoints << QPoint(p.x() - 1, p.y());
            if (p.y() + 1 < imageCracks->height())
                listePoints << QPoint(p.x(), p.y() + 1);
            if (p.y() - 1 >= 0)
                listePoints << QPoint(p.x(), p.y() - 1);
        }
    }

    return true;
}

bool Substrate::getGenerationCadencee() const
{
    return generationCadencee;
}

int Substrate::getChargement() const
{
    return chargement;
}

void Substrate::run()
{
    if (!boucleTerminee)
        return;

    boucleTerminee = false;

    int i = 0;
    while (!arret && nombreCracksFinis != nombreCracksFinal)
    {
        listeCracks[i++]->move();
        if (i >= listeCracks.size())
            i = 0;
        if (generationCadencee)
        {
            emit modifie();
            if (periodeCroissance > 0)
                msleep(periodeCroissance);
        }
    }
    if (!arret)
    {
        if (painterImage)
            painterImage->end();
        if (remplissageZones)
            remplirZones();
        if (!arret)
        {
            if ((!filDeFer || remplissageZones) && repassageFinalCracks)
                painterSubstrate->drawImage(0, 0, *imageCracks);
            nettoyer();
            estTermine = true;
            emit termine();
        }
    }

    if (arret)
        emit arrete();

    boucleTerminee = true;
}

void Substrate::setPixmapFond(const QPixmap &pixmap)
{
    pixmapFond = pixmap;
}

const QPixmap & Substrate::getPixmapFond() const
{
    return pixmapFond;
}

std::default_random_engine & Substrate::generateur(TypeGenerateur type)
{
    return generateurs[int(type)];
}

quint64 Substrate::getCleGenerateur(TypeGenerateur type) const
{
    return clesGenerateurs[int(type)];
}

void Substrate::setCleGenerateur(TypeGenerateur type, quint64 cle)
{
    clesGenerateurs[int(type)] = cle;
}

bool Substrate::getGenerateurForce(TypeGenerateur type) const
{
    return generateursForces[int(type)];
}

void Substrate::setGenerateurForce(TypeGenerateur type, bool b)
{
    generateursForces[int(type)] = b;
}

const QList<QColor> & Substrate::getCouleurs(TypeCouleur type) const
{
    return couleurs[int(type)];
}

const QColor & Substrate::getCouleurAleatoire(TypeCouleur type)
{
    return couleurs[int(type)][generateurs[2 + int(type)]() % couleurs[int(type)].size()];
}

uint Substrate::getNumeroSubstrate() const
{
    return numeroSubstrate;
}

bool Substrate::getEstTermine() const
{
    return estTermine;
}

void Substrate::nettoyer()
{
    if (painterSubstrate)
    {
        if (painterSubstrate->isActive())
            painterSubstrate->end();
        delete painterSubstrate;
        painterSubstrate = 0;
    }
    if (painterImage)
    {
        if (painterImage->isActive())
            painterImage->end();
        delete painterImage;
        painterImage = 0;
    }
    if (imageCracks)
    {
        delete imageCracks;
        imageCracks = 0;
    }
    for (int i = 0; i < listeCracks.size(); i++)
        delete listeCracks[i];
    listeCracks.clear();
    if (grilleCracks)
    {
        delete[] grilleCracks;
        grilleCracks = 0;
    }
}

void Substrate::incrementerNombreCracksDebutes()
{
    nombreCracksDebutes++;
}

bool Substrate::incrementerNombreCracksFinis(Crack *crack)
{
    double ch = (double)100.0*(++nombreCracksFinis)/nombreCracksFinal;
    m_penWidth = interpolation.calculerImage(ch);
    if (m_penWidth <= 0)
        m_penWidth = 1;
    if (chargement != (int)ch)
    {
        chargement = (int)ch;
        emit chargementChange(chargement);
    }
    if (nombreCracksDebutes == nombreCracksFinal)
    {
        listeCracks.removeOne(crack);
        crack->deleteLater();
        return true;
    }
    return false;
}

void Substrate::initialiserGenerateurs()
{
    for (int i = 0; i < NOMBRE_GENERATEURS; i++)
    {
        if (!generateursForces[i])
            clesGenerateurs[i] = (quint64)(((quint64)(rand()%12345) << 48) | ((quint64)(rand()%23451) << 32) | ((quint64)(rand()%34512) << 16) | (quint64)(rand()%45123));
        generateurs[i].seed(clesGenerateurs[i]);
    }
}

QThread::Priority Substrate::getPrioriteThread() const
{
    return prioriteThread;
}

void Substrate::setPrioriteThread(QThread::Priority priorite)
{
    prioriteThread = priorite;
}

bool Substrate::getAntialiasingActive() const
{
    return antialiasingActive;
}

void Substrate::setAntialiasingActive(bool b)
{
    antialiasingActive = b;
}

int Substrate::getProbabiliteDeboublementCrack() const
{
    return probabiliteDedoublementCrack;
}

void Substrate::setProbabiliteDeboublementCrack(int probabilite)
{
    probabiliteDedoublementCrack = probabilite;
}

int Substrate::getNombreCracksSimultanesEnCreation() const
{
    return nombreCracksSimultanesEnCreation;
}

void Substrate::setNombreCracksSimultanesEnCreation(int nb)
{
    nombreCracksSimultanesEnCreation = nb;
}

bool Substrate::getRepassageFinalCracks() const
{
    return repassageFinalCracks;
}

void Substrate::setRepassageFinalCracks(bool b)
{
    repassageFinalCracks = b;
}
