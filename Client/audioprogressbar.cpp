/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "audioprogressbar.h"

AudioProgressBar::AudioProgressBar(QWidget *parent) :
    QWidget(parent),
    pictureFilledProgressBar(":/images/bar_meter.png"),
    pictureEmptyProgressBar(":/images/bar_meter_empty.png")
{

}

void AudioProgressBar::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(0,0,pictureEmptyProgressBar);
}
