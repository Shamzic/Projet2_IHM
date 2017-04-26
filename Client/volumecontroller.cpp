/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "volumecontroller.h"

#include <QDebug>

/**
 * Constructeur VolumeController.
 *
 * <p>
 * volume initialisé à 3.
 * Inclu des images pour le slider et le fond et pour le mute.
 * </p>
 *
 * @param parent
 *            QWidget parent de la classe.
 */
VolumeController::VolumeController(QWidget *parent) :
    QWidget(parent),
    volume(50),
    audioControl(":/images/volumecontrol.png"),
    audioSlider(":/images/volumeslider.png")
{

}

void VolumeController::changeVolume(int v) {
    if (v<0) volume = v;
    if (v>100) volume = 100;
    update();
}

/**
 * Redessine l'image de fond du slider lors d'un évènement Qpaint.
 *
 * @param type QPaintEvent *
 *            Evenement QPaint.
 */
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

/**
 * Redessine l'image de fond du slider lors d'un évènement QMouse.
 *
 * @param event
 *            Evenement souris.
 */
void VolumeController::mousePressEvent(QMouseEvent *event) {
    float pourc = (float)(event->y())/(float)audioControl.height();
    volume = 100-floor(pourc*100);
    if (volume < 0 )
        volume = 0;
    else if (volume > 100)
        volume = 100;
    update();
    emit volumeChanged(volume);
}

void VolumeController::mouseMoveEvent(QMouseEvent *event) {
    mousePressEvent(event);
}
