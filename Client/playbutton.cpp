/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#include "playbutton.h"

#include <QDebug>

Playbutton::Playbutton(QWidget *parent) :
    QWidget(parent),
    isPlaying(false),
    picturePlayButton(":/images/playButton.png"),
    picturePauseButton(":/images/pauseButton.png")
{
}

void Playbutton::setState(bool playing) { isPlaying = playing; }

void Playbutton::playOrPause(bool playing) {
    setState(playing);
    update();
}

void Playbutton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    if (isPlaying)
        painter.drawPixmap(0,0,picturePauseButton);
    else
        painter.drawPixmap(0,0,picturePlayButton);
}

void Playbutton::mousePressEvent(QMouseEvent *event) {
    if (isPlaying)
        setState(false);
    else
        setState(true);
    update();
}

