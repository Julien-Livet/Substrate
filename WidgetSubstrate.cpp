#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QKeyEvent>

#include "WidgetSubstrate.h"
#include "Substrate.h"

WidgetSubstrate::WidgetSubstrate(Substrate *s) : QWidget()
{
    numeroSubstrate = 0;
    substrate = QPointer<Substrate>(s);
    setFixedSize(s->getRect().size()*s->getFacteurEchelle());
    pixmap = new QPixmap(size());
    pixmap->fill(QColor(0, 0, 0, 0));
    hide();
    s->setParent(this);
    connect(s, SIGNAL(arrete()), this, SLOT(deleteLater()));
    connect(s, SIGNAL(termine()), this, SLOT(creerActions()));

    setWindowIcon(QIcon(":/Images/Substrate.png"));
}

WidgetSubstrate::~WidgetSubstrate()
{
    if (substrate)
    {
        substrate->arreter();
        delete substrate;
    }
    if (!pixmap->isNull())
        delete pixmap;
}

void WidgetSubstrate::creerActions()
{
    update();

    QAction *actionEnregistrer = new QAction(tr("&Enregistrer comme image"), this);
    actionEnregistrer->setShortcut(QKeySequence::Save);
    QAction *actionCopier = new QAction(tr("&Copier comme image"), this);
    actionCopier->setShortcut(QKeySequence::Copy);

    addAction(actionEnregistrer);
    addAction(actionCopier);

    connect(actionEnregistrer, SIGNAL(triggered()), this, SLOT(enregistrerCommeImage()));
    connect(actionCopier, SIGNAL(triggered()), this, SLOT(copierCommeImage()));

    setContextMenuPolicy(Qt::ActionsContextMenu);

    numeroSubstrate = substrate->getNumeroSubstrate();

    delete substrate;

    substrate = 0;
}

void WidgetSubstrate::enregistrerCommeImage()
{
    QString texte = QFileDialog::getSaveFileName(this, tr("Choisir un fichier de destination"),
                                                 tr("Substrate n°%1.png").arg(numeroSubstrate),
                                                 tr("Images (*.bmp *.jpeg *.jpg *.png *.tiff)"));

    if (texte.isNull())
        return;

    pixmap->save(texte, 0, 100);
}

void WidgetSubstrate::copierCommeImage()
{
    QApplication::clipboard()->setPixmap(*pixmap);
}

QPixmap * WidgetSubstrate::getPixmap() const
{
    return pixmap;
}

void WidgetSubstrate::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    //painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, *pixmap);

    if (substrate)
    {
        if (!substrate->getEstTermine())
        {
            QFontMetrics fm(painter.font());
            QString const texte = tr("Génération en cours... (%1%)").arg(substrate->getChargement());
            QRectF r = fm.boundingRect(texte);
            r.setWidth(r.width()+10);
            r.moveTo(QPointF(width(), height())-QPointF(r.width(), r.height()));
            QColor c = Qt::black;//palette().color(QPalette::ToolTipBase);
            c.setAlpha(128);
            QPen pen = painter.pen();
            pen.setColor(c);
            painter.setPen(pen);
            painter.fillRect(r, c);
            c = Qt::white;//c = palette().color(QPalette::ToolTipText);
            c.setAlpha(128);
            pen.setColor(c);
            painter.setPen(pen);
            painter.drawText(r, Qt::AlignCenter, texte);
        }
    }
}

void WidgetSubstrate::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);

    deleteLater();
}

void WidgetSubstrate::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape)// && isFullScreen())
        close();
}
