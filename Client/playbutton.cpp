/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#include "playbutton.h"

Playbutton::Playbutton(QWidget *parent) :
    QWidget(parent),
    picturePlayButton(":/images/play.png")
{

}

/* ne pas oublier d'appeler update() quand on change qqch à l'image */

void Playbutton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(0,0,picturePlayButton);

}
