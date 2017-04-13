/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "audioprogressbar.h"

AudioProgressBar::AudioProgressBar(QWidget *parent) :
    QWidget(parent),
    timePassed(0),
    audioLength(2000), //pour tester
    pictureFilledProgressBar(":/images/bar_meter.png"),
    pictureEmptyProgressBar(":/images/bar_meter_empty.png")
{

}

void AudioProgressBar::changeAudio(int length) {
    timePassed = 0;
    audioLength = length;
}

void AudioProgressBar::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(0,0,pictureEmptyProgressBar);
    if (audioLength != 0) {
        float factor = (float)(pictureEmptyProgressBar.width())/(float)audioLength;
        painter.drawPixmap(0,0,factor*timePassed,11,pictureFilledProgressBar);
    }
}

void AudioProgressBar::mouseMoveEvent(QMouseEvent *event) {
    mousePressEvent(event);
}

void AudioProgressBar::mousePressEvent(QMouseEvent *event) {
    float pourc = (float)(event->x())/(float)pictureEmptyProgressBar.width();
    timePassed = floor(pourc*audioLength);
    if (timePassed < 0 )
        timePassed = 0;
    else if (timePassed > audioLength)
        timePassed = audioLength;
    update();
}
