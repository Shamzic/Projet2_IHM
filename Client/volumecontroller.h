/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef VOLUMECONTROLLER_H
#define VOLUMECONTROLLER_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

class VolumeController : public QWidget
{
    Q_OBJECT
public:
    explicit VolumeController(QWidget *parent = 0);

public slots:
    void changeVolume(int v);

signals:
    void volumeChanged(int v);

protected:
    int volume; //percent ?
    QPixmap audioControl;
    QPixmap audioSlider;
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // VOLUMECONTROLLER_H
