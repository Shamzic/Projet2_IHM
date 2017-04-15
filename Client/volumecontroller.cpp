/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "volumecontroller.h"

#include <QDebug>

VolumeController::VolumeController(QWidget *parent) :
    QWidget(parent),
    volume(3),
    audioControl(":/images/volumecontrol.png"),
    audioSlider(":/images/volumeslider.png"),
    muteSymbol(":/images/audio_off.png"),
    volumeOnSymbol(":/images/audio_on.png")
{

}

void VolumeController::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(0,0,audioControl);
    float factor = (float)volume/(float)100.0;
    int maxHeight = audioControl.height();
    if (volume < 7 ) {
        painter.drawPixmap(0,maxHeight-audioSlider.height(),audioSlider);
    } else {
        painter.drawPixmap(0,maxHeight-factor*maxHeight,audioSlider);
    }
}

void VolumeController::mousePressEvent(QMouseEvent *event) {
    float pourc = (float)(event->y())/(float)audioControl.height();
    volume = 100-floor(pourc*100);
    if (volume < 0 )
        volume = 0;
    else if (volume > 100)
        volume = 100;
    update();

    qDebug() << volume ;
}

void VolumeController::mouseMoveEvent(QMouseEvent *event) {
    mousePressEvent(event);
}
