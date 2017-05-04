/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "audioprogressbar.h"
#include <QDebug>

/**
 * Constructeur AudioProgressBar.
 *
 * <p>
 * Initilise le temps à 0.
 * Taille de la barre à 2000.
 * Inclu deux images de barre : vide/remplie.
 * </p>
 * @param parent
 *            Widget parent.
 */
AudioProgressBar::AudioProgressBar(QWidget *parent) :
    QWidget(parent),

    timePassed(0),
    audioLength(0),
    pictureFilledProgressBar(":/images/bar_meter.png"),
    pictureEmptyProgressBar(":/images/bar_meter_empty.png")
{

}

/**
 * Change le temps de la barre.
 * Temps mis à 0?
 *
 * @param length
 *            Nouvelle taille de la barre.
 */
void AudioProgressBar::changeAudio(int length) {
    timePassed = 0;
    if(length<0)
        timePassed=0;
    if(length>audioLength)
        timePassed=audioLength;
    timePassed=length;
    update();
}

/**
 * Evenement paint.
 *
 * <p>
 * Repeint la barre dès qu'un évênement QPaint est effectué.
 * Affiche barre vide si taille 0
 * Sinon,
 * </p>
 *
 * @param type QPaintEvent *
 *            Evenement de paint
 */
void AudioProgressBar::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(0,0,pictureEmptyProgressBar);
    if (audioLength != 0) {
        float factor = (float)(pictureEmptyProgressBar.width())/(float)audioLength;
        painter.drawPixmap(0,0,factor*timePassed,11,pictureFilledProgressBar);
    }
}

/**
 * Lance la fonction mousePressEvent
 * Si un evenement mouseEvent surgit.
 *
 * @param type QMouseEvent *event
 *            Evenement de souris.
 */
void AudioProgressBar::mouseMoveEvent(QMouseEvent *event) {
    if (audioLength == 0) return;
    float pourc = (float)(event->x())/(float)pictureEmptyProgressBar.width();
    timePassed = floor(pourc*audioLength);
    if (timePassed < 0 )
        timePassed = 0;
    else if (timePassed > audioLength)
        timePassed = audioLength;
    update();

    emit timeChanged(timePassed);
}

/**
 * Update la barre lorsqu'on presse la souris
 *
 * <p>
 * Décale la barre à la position du temps de lecture
 * indiqué par la position de la souris lorsqu'elle est pressée.
 * </p>
 *
 * @param type QMouseEvent *event
 *            Evenement de souris.
 */
void AudioProgressBar::mousePressEvent(QMouseEvent *event) {
    if (audioLength == 0) return;
    float pourc = (float)(event->x())/(float)pictureEmptyProgressBar.width();
    timePassed = floor(pourc*audioLength);
    if (timePassed < 0 )
        timePassed = 0;
    else if (timePassed > audioLength)
        timePassed = audioLength;
    update();
    emit timeChanged(timePassed);
}


void AudioProgressBar::changeMax(int timemax){
    this->audioLength=timemax;
}

void AudioProgressBar::addOneSecond() {
    changeAudio(timePassed+1);
    /** pas d'émit ici ! */
}
