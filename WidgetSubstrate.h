#ifndef WIDGETSUBSTRATE_H
#define WIDGETSUBSTRATE_H

#include <QWidget>
#include <QPointer>

class Substrate;

class WidgetSubstrate : public QWidget
{
    Q_OBJECT

    public:
        WidgetSubstrate(Substrate *s);
        ~WidgetSubstrate();
        QPixmap *getPixmap() const;

    private:
        QPointer<Substrate> substrate;
        QPixmap *pixmap;
        int numeroSubstrate;

    private slots:
        void creerActions();
        void enregistrerCommeImage();
        void copierCommeImage();

    protected:
        void paintEvent(QPaintEvent *event);
        void closeEvent(QCloseEvent *event);
        void keyPressEvent(QKeyEvent *event);
};

#endif // WIDGETSUBSTRATE_H
