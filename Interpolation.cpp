#include <cmath>

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QToolTip>
#include <QScreen>

#include <qwt_plot_canvas.h>
#include <qwt_symbol.h>

#include <Eigen/Dense>

#include "Interpolation.h"

class Dialog : public QDialog
{
    public:
        Dialog(QWidget *parent = 0) : QDialog(parent)
        {
        }

    protected:
        void keyPressEvent(QKeyEvent *event)
        {
            if (event->key() != Qt::Key_Escape)
                QDialog::keyPressEvent(event);
        }
};

BoiteCoordonnees::BoiteCoordonnees(double x, double y, QwtPlot *p) : QWidget()
{
    lineEditX = new QLineEdit(this);
    lineEditX->setText(QString::number(x));
    QDoubleValidator *vX = new QDoubleValidator(this);
    vX->setBottom(p->axisInterval(QwtPlot::xBottom).minValue());
    vX->setTop(p->axisInterval(QwtPlot::xBottom).maxValue());
    lineEditX->setValidator(vX);
    lineEditY = new QLineEdit(this);
    lineEditY->setText(QString::number(y));
    QDoubleValidator *vY = new QDoubleValidator(this);
    vY->setBottom(p->axisInterval(QwtPlot::yLeft).minValue());
    vY->setTop(p->axisInterval(QwtPlot::yLeft).maxValue());
    lineEditY->setValidator(vY);

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(lineEditX, SIGNAL(returnPressed()), this, SLOT(accept()));
    connect(lineEditY, SIGNAL(returnPressed()), this, SLOT(accept()));

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("x = "), this), 0, 0);
    gridLayout->addWidget(lineEditX, 0, 1);
    gridLayout->addWidget(new QLabel(tr("y = "), this), 1, 0);
    gridLayout->addWidget(lineEditY, 1, 1);
    gridLayout->addWidget(frame, 2, 0, 1, 2);
    gridLayout->addWidget(dialogButtonBox, 3, 0, 1, 2);
    setLayout(gridLayout);

    setWindowFlags(Qt::Popup);
}

double BoiteCoordonnees::getX() const
{
    return lineEditX->text().toDouble();
}

double BoiteCoordonnees::getY() const
{
    return lineEditY->text().toDouble();
}

int BoiteCoordonnees::exec()
{
    r = 0;
    show();

    while (!r)
        QApplication::processEvents();

    close();

    if (r == 2)
        return QDialog::Rejected;

    return QDialog::Accepted;
}

void BoiteCoordonnees::accept()
{
    r = 1;
}

void BoiteCoordonnees::reject()
{
    r = 2;
}

void BoiteCoordonnees::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);

    if (!r)
        reject();
}

CanvasPicker::CanvasPicker(QwtPlot *plot): QObject(plot)
{
    pm = 0;

    QWidget *canvas = plot->canvas();

    canvas->installEventFilter(this);

    // We want the focus, but no focus rect. The
    // selected point will be highlighted instead.

    canvas->setFocusPolicy(Qt::StrongFocus);
#ifndef QT_NO_CURSOR
    canvas->setCursor(Qt::PointingHandCursor);
#endif
    canvas->setFocus();
}

bool CanvasPicker::eventFilter(QObject *object, QEvent *e)
{
    if (object != (QObject *)plot()->canvas())
        return false;

    switch (e->type())
    {
        case QEvent::FocusIn:
            showCursor(true);
            break;
        case QEvent::FocusOut:
            showCursor(false);
            pm = 0;
            break;
        case QEvent::MouseButtonPress:
        {
            switch (((QMouseEvent *)e)->button())
            {
                case Qt::LeftButton:
                {
                    select(((QMouseEvent *)e)->pos());
                    if (pm)
                        oldPos = QPointF(pm->xValue(), pm->yValue());
                    else
                        oldPos = QPointF(-1, -1);
                    break;
                }
                case Qt::RightButton:
                    add(((QMouseEvent *)e)->pos());
                    break;
                default:
                    break;
            }
            return true;
        }
        case QEvent::MouseButtonDblClick:
        {
            if (((QMouseEvent *)e)->button() == Qt::LeftButton)
            {
                select(((QMouseEvent *)e)->pos());
                if (pm)
                {
                    QwtPlotMarker *p = pm;
                    BoiteCoordonnees b(pm->xValue(), pm->yValue(), plot());
                    b.move(QCursor::pos());
                    if (b.exec() == QDialog::Accepted)
                    {
                        pm = p;
                        plot()->canvas()->setFocus();
                        showCursor(true);
                        if (b.getX() != pm->xValue() || b.getY() != pm->yValue())
                        {
                            pm->setValue(b.getX(), b.getY());
                            actualiser();
                        }
                    }
                }
            }
            return true;
        }
        case QEvent::MouseButtonRelease:
            if (((QMouseEvent *)e)->button() == Qt::LeftButton || ((QMouseEvent *)e)->button() == Qt::MiddleButton)
                actualiser();
            return true;
        case QEvent::MouseMove:
        {
            if (((QMouseEvent *)e)->buttons() == Qt::LeftButton || ((QMouseEvent *)e)->buttons() == Qt::MiddleButton)
                move(((QMouseEvent *)e)->pos());
            return true;
        }
        case QEvent::KeyPress:
        {
            const int delta = 5;
            switch(((const QKeyEvent *)e)->key())
            {
                case Qt::Key_Delete:
                    remove();
                    break;
                case Qt::Key_1:
                    moveBy(-delta, -delta);
                    break;
                case Qt::Key_Down:
                case Qt::Key_2:
                    moveBy(0, -delta);
                    break;
                case Qt::Key_3:
                    moveBy(delta, -delta);
                    break;
                case Qt::Key_Left:
                case Qt::Key_4:
                    moveBy(-delta, 0);
                    break;
                case Qt::Key_Right:
                case Qt::Key_6:
                    moveBy(delta, 0);
                    break;
                case Qt::Key_7:
                    moveBy(-delta, delta);
                    break;
                case Qt::Key_Up:
                case Qt::Key_8:
                    moveBy(0, delta);
                    break;
                case Qt::Key_9:
                    moveBy(delta, delta);
                    break;
                case Qt::Key_Escape:
                    if (pm && oldPos != QPointF(-1, -1))
                    {
                        pm->setXValue(oldPos.x());
                        pm->setYValue(oldPos.y());
                        actualiser();
                        oldPos = QPointF(-1, -1);
                        showCursor(false);
                        pm = 0;
                        QToolTip::hideText();
                    }
                    break;
                default:
                    break;
            }
        }
        default:
            break;
    }

    return QObject::eventFilter(object, e);
}

QwtPlot * CanvasPicker::plot() const
{
    return qobject_cast<QwtPlot *>(parent());
}

void CanvasPicker::select(const QPoint &pos)
{
    QwtPlotMarker *p = 0;
    double dist = 1e100;

    QList<QwtPlotMarker *> liste = qobject_cast<Graphique *>(plot())->getListePlotMarkers();
    for (int i = 0; i < liste.size(); i++)
    {
        QwtPlotMarker *pBis = liste[i];
        double d = sqrt(pow(pos.x()-plot()->transform(QwtPlot::xBottom, pBis->xValue()), 2)+pow(pos.y()-plot()->transform(QwtPlot::yLeft, pBis->yValue()), 2));
        if (d < dist)
        {
            p = pBis;
            dist = d;
        }
    }

    showCursor(false);
    pm = 0;

    if (p && dist < 10) // 10 pixels tolerance
    {
        pm = p;
        showCursor(true);
        QToolTip::showText(plot()->mapToGlobal(QPoint(plot()->transform(QwtPlot::xBottom, pm->xValue()), plot()->transform(QwtPlot::yLeft, p->yValue()))), QString("x = ")+QString::number(pm->xValue())+QString("\ny = ")+QString::number(pm->yValue()), plot());
    }
    else
        QToolTip::hideText();
}

void CanvasPicker::moveBy(int dx, int dy)
{
    if (dx == 0 && dy == 0)
        return;

    if (!pm)
        return;

    double x = pm->xValue()+dx*plot()->axisStepSize(QwtPlot::xBottom)/20;
    double y = pm->yValue()+dy*plot()->axisStepSize(QwtPlot::yLeft)/20;

    if (x < plot()->axisInterval(QwtPlot::xBottom).minValue())
        x = plot()->axisInterval(QwtPlot::xBottom).minValue();
    if (x > plot()->axisInterval(QwtPlot::xBottom).maxValue())
        x = plot()->axisInterval(QwtPlot::xBottom).maxValue();
    if (y < plot()->axisInterval(QwtPlot::yLeft).minValue())
        y = plot()->axisInterval(QwtPlot::yLeft).minValue();
    if (y > plot()->axisInterval(QwtPlot::yLeft).maxValue())
        y = plot()->axisInterval(QwtPlot::yLeft).maxValue();

    pm->setValue(x, y);
    QToolTip::showText(plot()->mapToGlobal(QPoint(plot()->transform(QwtPlot::xBottom, pm->xValue()), plot()->transform(QwtPlot::yLeft, pm->yValue()))), QString("x = ")+QString::number(x)+QString("\ny = ")+QString::number(y), plot());

    actualiser();
}

void CanvasPicker::move(const QPoint &pos)
{
    if (!pm)
        return;

    double x = plot()->invTransform(QwtPlot::xBottom, pos.x());
    double y = plot()->invTransform(QwtPlot::yLeft, pos.y());

    if (x < plot()->axisInterval(QwtPlot::xBottom).minValue())
        x = plot()->axisInterval(QwtPlot::xBottom).minValue();
    if (x > plot()->axisInterval(QwtPlot::xBottom).maxValue())
        x = plot()->axisInterval(QwtPlot::xBottom).maxValue();
    if (y < plot()->axisInterval(QwtPlot::yLeft).minValue())
        y = plot()->axisInterval(QwtPlot::yLeft).minValue();
    if (y > plot()->axisInterval(QwtPlot::yLeft).maxValue())
        y = plot()->axisInterval(QwtPlot::yLeft).maxValue();

    pm->setValue(x, y);
    QToolTip::showText(plot()->mapToGlobal(QPoint(plot()->transform(QwtPlot::xBottom, pm->xValue()), plot()->transform(QwtPlot::yLeft, pm->yValue()))), QString("x = ")+QString::number(x)+QString("\ny = ")+QString::number(y), plot());

    //plot()->replot();
    actualiser();
}

void CanvasPicker::showCursor(bool showIt)
{
    if (!pm)
        return;

    QwtSymbol *symbol = const_cast<QwtSymbol *>(pm->symbol());

    if (showIt)
        symbol->setBrush(Qt::red);
    else
        symbol->setBrush(Qt::gray);

    plot()->replot();
}

void CanvasPicker::add(const QPoint &pos)
{
    Graphique *g = qobject_cast<Graphique *>(plot());

    QPointF p = QPointF(plot()->invTransform(QwtPlot::xBottom, pos.x()), plot()->invTransform(QwtPlot::yLeft, pos.y()));

    bool b = g->getInterpolation()->ajouterPoint(p);

    if (b)
    {
        g->actualiser();

        QList<QwtPlotMarker *> liste = g->getListePlotMarkers();
        for (int i = 0; i < liste.size(); i++)
        {
            if (liste[i]->value() == p)
            {
                pm = liste[i];
                showCursor(true);
                break;
            }
        }

        plot()->replot();
    }
}

void CanvasPicker::remove()
{
    if (!pm)
        return;

    Graphique *g = qobject_cast<Graphique *>(plot());

    bool b = g->getInterpolation()->supprimerPoint(pm->value());

    if (b)
    {
        g->actualiser();

        pm = 0;

        plot()->replot();
    }
}

void CanvasPicker::actualiser()
{
    if (!pm)
        return;

    QPointF p = pm->value();

    Graphique *g = qobject_cast<Graphique *>(plot());

    QList<QPointF> l;
    QList<QwtPlotMarker *> liste = g->getListePlotMarkers();

    for (int i = 0; i < liste.size(); i++)
        l << liste[i]->value();

    g->getInterpolation()->setListePoints(l);

    g->actualiser();

    liste = g->getListePlotMarkers();
    for (int i = 0; i < liste.size(); i++)
    {
        if (liste[i]->value() == p)
        {
            pm = liste[i];
            showCursor(true);
            break;
        }
    }

    plot()->replot();
}

Graphique::Graphique(Interpolation *i, QWidget *parent) : QwtPlot(parent)
{
    interpolation = i;

    setAxisTitle(QwtPlot::yLeft, QString("Taille des cracks"));
    setAxisScale(QwtPlot::yLeft, 0, 10, 1);
    setAxisTitle(QwtPlot::xBottom, QString("Pourcentage d'avancement"));
    setAxisScale(QwtPlot::xBottom, 0, 100, 10);
    setCanvasBackground(Qt::white);

    courbe = new QwtPlotCurve;
    QPen p = courbe->pen();
    p.setStyle(Qt::SolidLine);
    courbe->setPen(p);
    courbe->attach(this);

    canvasPicker = new CanvasPicker(this);

    actualiser();
}

Graphique::~Graphique()
{
    delete canvasPicker;
    for (int i = 0; i < listePlotMarkers.size(); i++)
    {
        listePlotMarkers[i]->detach();
        delete listePlotMarkers[i];
    }
    courbe->detach();
    delete courbe;
}

void Graphique::actualiser()
{
    for (int i = 0; i < listePlotMarkers.size(); i++)
    {
        listePlotMarkers[i]->detach();
        delete listePlotMarkers[i];
    }
    listePlotMarkers.clear();

    QList<QPointF> l = interpolation->getListePoints();

    for (int i = 0; i < l.size(); i++)
    {
        QwtPlotMarker *pm = new QwtPlotMarker;
        pm->setValue(l[i]);
        QwtSymbol *symbole = new QwtSymbol(QwtSymbol::Ellipse);
        symbole->setSize(5, 5);
        symbole->setBrush(Qt::gray);
        pm->setSymbol(symbole);
        pm->attach(this);
        listePlotMarkers << pm;
    }

    QVector<double> xData, yData;

    auto const w{QGuiApplication::screenAt(pos())->size().width()};

    for (int i = 0; i < w; i++)
    {
        xData << (double)i / w * 100;
        yData << interpolation->calculerImage(xData.last());
    }

    courbe->setSamples(xData, yData);

    replot();
}

void Graphique::changerInterpolation(int i)
{
    interpolation->setType(Interpolation::Type(i));

    actualiser();
}

Interpolation * Graphique::getInterpolation() const
{
    return interpolation;
}

QList<QwtPlotMarker *> Graphique::getListePlotMarkers() const
{
    return listePlotMarkers;
}

Interpolation::Interpolation()
{
   setType(Constante, QList<QPointF>() << QPointF(0,1));
}

Interpolation::Type Interpolation::getType() const
{
    return type;
}

void Interpolation::setType(Type t, const QList<QPointF> &l)
{
    if (type == t && listePoints == l)
        return;

    type = t;

    setListePoints(l);
}

const QList<QPointF> &Interpolation::getListePoints() const
{
    return listePoints;
}

void Interpolation::setListePoints(QList<QPointF> l)
{
    if (l.isEmpty())
        l = listePoints;

    switch (type)
    {
        case Constante:
            if (l.isEmpty())
                l << QPointF(0, 1);
            break;
        case Affine:
            if (l.size() < 2)
            {
                l.clear();
                l << QPointF(0, 1) << QPointF(100, 0);
            }
            break;
        case Parabolique:
            if (l.size() < 3)
            {
                l.clear();
                l << QPointF(0, 4) << QPointF(50, 1) << QPointF(100, 0);
            }
            break;
        case Cubique:
            if (l.size() < 4)
            {
                l.clear();
                l << QPointF(0, 8) << QPointF(50, 1) << QPointF(75, 0.125) << QPointF(100, 0);
            }
            break;
        case Exponentielle:
            if (l.size() < 2)
            {
                l.clear();
                l << QPointF(0, 1) << QPointF(100, 0.1);
            }
            break;
        case Logarithmique:
            if (l.size() < 2)
            {
                l.clear();
                l << QPointF(1, 1) << QPointF(100, 2);
            }
            break;
        case Hyperbole:
            if (l.size() < 2)
            {
                l.clear();
                l << QPointF(0.1, 1) << QPointF(100, 0.1);
            }
            break;
        case Puissance:
            if (l.size() < 2)
            {
                l.clear();
                l << QPointF(0.1, 5) << QPointF(100, 0.1);
            }
            break;
        case Sinus:
            if (l.size() < 4)
            {
                l.clear();
                for (double i = 0; i < 100; i += 100/4)
                    l << QPointF(i, sin(i*M_PI/100)+1);
            }
            break;
        case ConstanteParMorceaux:
            if (l.isEmpty())
                l << QPointF(0, 1);
            break;
        case AffineParMorceaux:
            if (l.size() < 2)
            {
                l.clear();
                l << QPointF(0, 1) << QPointF(100, 0);
            }
            break;
        case Polynomiale:
            if (l.isEmpty())
                l << QPointF(0, 1);
            break;
        default:
            break;
    }

    QList<QPointF> liste;
    QList<double> listeX;

    for (int i = 0; i < l.size(); i++)
    {
        if (!listeX.contains(l[i].x()))
            listeX << l[i].x();
        else
        {
            l.removeAt(i);
            i--;
        }
    }

    std::sort(listeX.begin(), listeX.end());

    for (int i = 0; i < listeX.size(); i++)
    {
        for (int j = 0; j < l.size(); j++)
        {
            if (listeX[i] == l[j].x())
            {
                liste << l[j];
                break;
            }
        }
    }

    listePoints = liste;

    calculerCoefficients();
}

bool Interpolation::ajouterPoint(const QPointF &p)
{
    if (listePoints.contains(p))
        return false;

    if (listePoints.isEmpty())
        listePoints << p;
    else
    {
        bool b = false;
        if (listePoints.first().x() == p.x())
            return false;
        else if (p.x() < listePoints.first().x())
        {
            b = true;
            listePoints.insert(0, p);
        }
        if (!b)
        {
            for (int i = 0; i < listePoints.size()-1; i++)
            {
                if (listePoints[i].x() == p.x())
                    return false;
                else if (listePoints[i].x() < p.x() && p.x() < listePoints[i+1].x())
                {
                    b = true;
                    listePoints.insert(i+1, p);
                    break;
                }
            }
            if (!b)
            {
                if (listePoints.last().x() == p.x())
                    return false;
                else
                    listePoints << p;
            }
        }
    }

    calculerCoefficients();

    return true;
}

bool Interpolation::supprimerPoint(const QPointF &p)
{
    switch (type)
    {
        case Constante:
            if (listePoints.size() <= 1)
                return false;
            break;
        case Affine:
            if (listePoints.size() <= 2)
                return false;
            break;
        case Parabolique:
            if (listePoints.size()<= 3)
                return false;
            break;
        case Cubique:
            if (listePoints.size() <= 4)
                return false;
            break;
        case Exponentielle:
            if (listePoints.size() <= 2)
                return false;
            break;
        case Logarithmique:
            if (listePoints.size() <= 2)
                return false;
            break;
        case Hyperbole:
            if (listePoints.size() <= 2)
                return false;
            break;
        case Puissance:
            if (listePoints.size() <= 2)
                return false;
            break;
        case Sinus:
            if (listePoints.size() <= 4)
                return false;
            break;
        case ConstanteParMorceaux:
            if (listePoints.size() <= 1)
                return false;
            break;
        case AffineParMorceaux:
            if (listePoints.size() <= 2)
                return false;
            break;
        case Polynomiale:
            if (listePoints.size() <= 1)
                return false;
            break;
        default:
            break;
    }

    bool b = listePoints.removeOne(p);

    if (b)
        calculerCoefficients();

    return b;
}

void Interpolation::calculerCoefficients()
{
    listeCoefficients.clear();

    switch (type)
    {
        case Constante:
        {
            double a = 0;
            for (int i = 0; i < listePoints.size(); i++)
                a += listePoints[i].y();
            a /= listePoints.size();
            listeCoefficients << a;
            break;
        }
        case Affine:
        {
            double sx = 0, sx2 = 0, sxy = 0, sy = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                sx += listePoints[i].x();
                sx2 += pow(listePoints[i].x(), 2);
                sxy += listePoints[i].x() * listePoints[i].y();
                sy += listePoints[i].y();
            }
            double a = 0;
            for (int i = 0; i < n; i++)
                a += listePoints[i].x() * (listePoints[i].y() - sy / n);
            a /= sx2 - pow(sx, 2) / n;
            listeCoefficients << a;
            listeCoefficients << (sy - a * sx) / n;
            break;
        }
        case Parabolique:
        {
            double sx = 0, sx2 = 0, sx3 = 0, sx4 = 0, sxy = 0, sx2y = 0, sy = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                const double &x = listePoints[i].x();
                const double &y = listePoints[i].y();
                sx += x;
                sx2 += pow(x, 2);
                sx3 += pow(x, 3);
                sx4 += pow(x, 4);
                sxy += x * y;
                sx2y += pow(x, 2) * y;
                sy += y;
            }
            Eigen::Matrix<double, 3, 3> matrice;
            matrice << sx4, sx3, sx2,
                       sx3, sx2, sx,
                       sx2, sx,  n;
            Eigen::Matrix<double, 3, 1> v;
            v << sx2y, sxy, sy;
            for (auto const& c : (matrice.inverse() * v).transpose().col(0))
                listeCoefficients << c;
            break;
        }
        case Cubique:
        {
            double sx = 0, sx2 = 0, sx3 = 0, sx4 = 0, sx5 = 0, sx6 = 0;
            double sy = 0, sxy = 0, sx2y = 0, sx3y = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                const double &x = listePoints[i].x();
                const double &y = listePoints[i].y();
                sx += x;
                sx2 += pow(x, 2);
                sx3 += pow(x, 3);
                sx4 += pow(x, 4);
                sx5 += pow(x, 5);
                sx6 += pow(x, 6);
                sy += y;
                sxy += x * y;
                sx2y += pow(x, 2) * y;
                sx3y += pow(x, 3) * y;
            }
            Eigen::Matrix<double, 4, 4> matrice;
            matrice << sx6, sx5, sx4, sx3,
                       sx5, sx4, sx3, sx2,
                       sx4, sx3, sx2, sx,
                       sx3, sx2, sx,  n;
            Eigen::Matrix<double, 4, 1> v;
            v << sx3y, sx2y, sxy, sy;
            for (auto const& c : (matrice.inverse() * v).transpose().col(0))
                listeCoefficients << c;
            break;
        }
        case Exponentielle:
        {
            double sx = 0, sx2 = 0, sxy = 0, sy = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                const double &x = listePoints[i].x();
                double y = log(listePoints[i].y());
                sx += x;
                sx2 += pow(x, 2);
                sxy += x * y;
                sy += y;
            }
            double a = 0;
            for (int i = 0; i < n; i++)
                a += listePoints[i].x() * (log(listePoints[i].y()) - sy / n);
            a /= sx2 - pow(sx, 2) / n;
            listeCoefficients << exp((sy - a * sx) / n);
            listeCoefficients << a;
            break;
        }
        case Logarithmique:
        {
            double sx = 0, sx2 = 0, sxy = 0, sy = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                double x = log(listePoints[i].x());
                const double &y = listePoints[i].y();
                sx += x;
                sx2 += pow(x, 2);
                sxy += x * y;
                sy += y;
            }
            double a = 0;
            for (int i = 0; i < n; i++)
                a += log(listePoints[i].x()) * (listePoints[i].y() - sy / n);
            a /= sx2 - pow(sx, 2) / n;
            listeCoefficients << (sy - a * sx) / n;
            listeCoefficients << a;
            break;
        }
        case Hyperbole:
        {
            double sx = 0, sx2 = 0, sxy = 0, sy = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                double x = 1/listePoints[i].x();
                const double &y = listePoints[i].y();
                sx += x;
                sx2 += pow(x, 2);
                sxy += x * y;
                sy += y;
            }
            double a = 0;
            for (int i = 0; i < n; i++)
                a += 1/listePoints[i].x() * (listePoints[i].y() - sy / n);
            a /= sx2 - pow(sx, 2) / n;
            listeCoefficients << a;
            listeCoefficients << (sy - a * sx) / n;
            break;
        }
        case Puissance:
        {
            double sx = 0, sx2 = 0, sxy = 0, sy = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                double x = log(listePoints[i].x());
                double y = log(listePoints[i].y());
                sx += x;
                sx2 += pow(x, 2);
                sxy += x * y;
                sy += y;
            }
            double a = 0;
            for (int i = 0; i < n; i++)
                a += log(listePoints[i].x()) * (log(listePoints[i].y()) - sy / n);
            a /= sx2 - pow(sx, 2) / n;
            listeCoefficients << exp((sy - a * sx) / n);
            listeCoefficients << a;
            break;
        }
        case Sinus:
        {
            double w1 = 0, a1 = 0, b1 = 0, c1 = 0;
            double A1 = 0, B1 = 0, C1 = 0, D1 = 0, Sk = 0, SSk = 0;
            double sSSk2 = 0, sxSSk = 0, sx2SSk = 0, sSSk = 0;
            double sx = 0, sx2 = 0, sx3 = 0, sx4 = 0;
            double sySSk = 0, sx2y = 0, sxy = 0, sy = 0;
            int n = listePoints.size();
            for (int i = 0; i < n; i++)
            {
                const double &x = listePoints[i].x();
                const double &y = listePoints[i].y();
                sx += x;
                sx2 += pow(x, 2);
                sx3 += pow(x, 3);
                sx4 += pow(x, 4);
                sy += y;
                sxy += x * y;
                sx2y += pow(x, 2) * y;
                if (i)
                {
                    double Sk1 = Sk;
                    Sk += (y + listePoints[i - 1].y()) * (x - listePoints[i - 1].x()) / 2;
                    SSk += (Sk + Sk1) * (x - listePoints[i - 1].x()) / 2;
                }
                sSSk2 += pow(SSk, 2);
                sxSSk += x * SSk;
                sSSk += SSk;
                sx2SSk += pow(x, 2) * SSk;
                sySSk += y * SSk;
            }
            Eigen::Matrix<double, 4, 4> matrice;
            matrice << sSSk2,  sx2SSk, sxSSk, sSSk,
                       sx2SSk, sx4,    sx3,   sx2,
                       sxSSk,  sx3,    sx2,   sx,
                       sSSk,   sx2,    sx,    n;
            Eigen::Matrix<double, 4, 1> v;
            v << sySSk, sx2y, sxy, sy;
            auto const result{matrice.inverse() * v};
            A1 = result[0];
            B1 = result[1];
            C1 = result[2];
            D1 = result[3];
            w1 = sqrt(-A1);
            a1 = -2.0 * B1 / A1;
            const double &x1 = listePoints.first().x();
            b1 = (B1 * pow(x1, 2) + C1 * x1 + D1 - a1) * sin(w1 * x1) + (C1 + 2.0 * B1 * x1) * cos(w1 * x1) / w1;
            c1 = (B1 * pow(x1, 2) + C1 * x1 + D1 - a1) * cos(w1 * x1) - (C1 + 2.0 * B1 * x1) * sin(w1 * x1) / w1;
            listeCoefficients << a1;
            listeCoefficients << b1;
            listeCoefficients << w1;
            listeCoefficients << c1;
            break;
        }
        case ConstanteParMorceaux:
        {
            for (int i = 0; i < listePoints.size(); i++)
                listeCoefficients << listePoints[i].y();
            break;
        }
        case AffineParMorceaux:
        {
            if (listePoints.size() == 1)
                listeCoefficients << 0 << listePoints.first().y();
            else
            {
                for (int i = 0; i < listePoints.size()-1; i++)
                {
                    listeCoefficients << (listePoints[i+1].y() - listePoints[i].y()) / (listePoints[i + 1].x() - listePoints[i].x());
                    listeCoefficients << listePoints[i].y() - listeCoefficients.last() * listePoints[i].x();
                }
            }
            break;
        }
        case Polynomiale:
            break;
        default:
            break;
    }
}

double Interpolation::calculerImage(double x)
{
    double y = 0;

    switch (type)
    {
        case Constante:
            y = listeCoefficients.first();
            break;
        case Affine:
            y = listeCoefficients[0] * x + listeCoefficients[1];
            break;
        case Parabolique:
            y = listeCoefficients[0] * pow(x, 2) + listeCoefficients[1] * x + listeCoefficients[2];
            break;
        case Cubique:
            y = listeCoefficients[0] * pow(x, 3) + listeCoefficients[1] * pow(x, 2)
                + listeCoefficients[2] * x + listeCoefficients[3];
            break;
        case Exponentielle:
            y = listeCoefficients[0] * exp(listeCoefficients[1] * x);
            break;
        case Logarithmique:
            if (!x)
                y = 0;
            else
                y = listeCoefficients[0] + listeCoefficients[1] * log(x);
            break;
        case Hyperbole:
            if (!x)
                y = 0;
            else
                y = listeCoefficients[0] / x + listeCoefficients[1];
            break;
        case Puissance:
            if (listeCoefficients[1] < 0 && !x)
                y = 0;
            else
                y = listeCoefficients[0] * pow(x, listeCoefficients[1]);
            break;
        case Sinus:
            y = listeCoefficients[0] + listeCoefficients[1] * sin(listeCoefficients[2] * x)
                + listeCoefficients[3] * cos(listeCoefficients[2] * x);
            break;
        case ConstanteParMorceaux:
        {
            if (listePoints.size() == 1)
                y = listeCoefficients.first();
            else
            {
                bool b = false;
                if (x < listePoints.first().x())
                {
                    y = listeCoefficients.first();
                    b = true;
                }
                if (!b)
                {
                    for (int i = 0; i < listePoints.size() - 1; i++)
                    {
                        if (listePoints[i].x() <= x && x < listePoints[i + 1].x())
                        {
                            y = listeCoefficients[i];
                            b = true;
                            break;
                        }
                    }
                    if (!b)
                        y = listeCoefficients.last();
                }
            }
            break;
        }
        case AffineParMorceaux:
        {
            bool b = false;
            if (x < listePoints.first().x())
            {
                y = listeCoefficients[0] * x + listeCoefficients[1];
                b = true;
            }
            if (!b)
            {
                for (int i = 0; i < listePoints.size() - 1; i++)
                {
                    if (listePoints[i].x() <= x && x < listePoints[i + 1].x())
                    {
                        y = listeCoefficients[i * 2] * x + listeCoefficients[i * 2 + 1];
                        b = true;
                        break;
                    }
                }
                if (!b)
                    y = listeCoefficients[listeCoefficients.size() - 2] * x
                        + listeCoefficients[listeCoefficients.size() - 1];
            }
            break;
        }
        case Polynomiale:
        {
            for (int i = 0; i < listePoints.size(); i++)
            {
                double a = listePoints[i].y();
                if (listePoints[i].x() == x)
                {
                    y = listePoints[i].y();
                    break;
                }
                if (a)
                {
                    for (int j = 0; j < listePoints.size(); j++)
                        if (i != j)
                            a *= (x - listePoints[j].x()) / (listePoints[i].x() - listePoints[j].x());
                    y += a;
                }
            }
            break;
        }
        default:
            break;
    }

    if (fabs(y) == NAN)
        y = 0;

    return y;
}

int Interpolation::exec(QWidget *parent)
{
    Dialog dialog(parent);
    dialog.setWindowTitle(QString("Interpolation"));

    graphique = new Graphique(this, &dialog);

    QFrame frame(&dialog);
    frame.setFrameShape(QFrame::HLine);
    frame.setFrameShadow(QFrame::Raised);

    QLabel labelExplication(QString("Faites un clic droit pour ajouter un point au graphique et faites un double-clic "
                                    "sur un point pour éditer ses coordonnées de façon plus précise."), &dialog);
    labelExplication.setWordWrap(true);

    QFrame frameBis(&dialog);
    frameBis.setFrameShape(QFrame::HLine);
    frameBis.setFrameShadow(QFrame::Raised);

    QComboBox comboBoxInterpolation(&dialog);
    comboBoxInterpolation.addItems(QStringList() << QString("Constante") << QString("Affine") << QString("Parabolique") << QString("Cubique") <<
                                   QString("Exponentielle") << QString("Logarithmique") << QString("Hyperbole") << QString("Puissance") <<
                                   QString("Sinus") << QString("Constante par morceaux") << QString("Affine par morceaux") <<
                                   QString("Polynomiale"));
    comboBoxInterpolation.setCurrentIndex(type);

    QObject::connect(&comboBoxInterpolation, SIGNAL(currentIndexChanged(int)), graphique, SLOT(changerInterpolation(int)));

    QDialogButtonBox dialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);

    QObject::connect(&dialogButtonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&dialogButtonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    QGridLayout gridLayout;
    gridLayout.addWidget(graphique, 0, 0, 1, 4);
    gridLayout.addWidget(&frame, 1, 0, 1, 4);
    gridLayout.addWidget(&labelExplication, 2, 0, 1, 4);
    gridLayout.addWidget(&frameBis, 3, 0, 1, 4);
    QLabel label(QString("Type de la courbe :"), &dialog);
    gridLayout.addWidget(&label, 4, 0);
    gridLayout.addWidget(&comboBoxInterpolation, 4, 1);
    gridLayout.addWidget(&dialogButtonBox, 4, 3);
    dialog.setLayout(&gridLayout);

    Type t = type;
    QList<QPointF> lP = listePoints;
    QList<double> lC = listeCoefficients;

    int r = dialog.exec();

    if (r == QDialog::Accepted)
    {
        type = Type(comboBoxInterpolation.currentIndex());
    }
    else
    {
        type = t;
        listePoints = lP;
        listeCoefficients = lC;
    }

    graphique->deleteLater();

    return r;
}


void Interpolation::enregistrer(QDataStream &out) const
{
    out << int(type);
    out << listePoints;
    out << listeCoefficients;
}

void Interpolation::charger(QDataStream &in, const QString &version)
{
    listePoints.clear();
    listeCoefficients.clear();
    int entier;
    in >> entier;
    type = Type(entier);
    in >> listePoints;
    in >> listeCoefficients;
}
