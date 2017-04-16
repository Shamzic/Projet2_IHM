/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#include "playbutton.h"

#include <QDebug>

/**
 * Constructeur Playbutton.
 *
 * <p>
 * isPlaying de base initialisé à "pause" (bool false).
 * Inclu deux images du bouton en mode pause et en mode play.
 * </p>
 *
 * @param parent
 *            QWidget parent de la classe.
 */
Playbutton::Playbutton(QWidget *parent) :
    QWidget(parent),
    isPlaying(false),
    picturePlayButton(":/images/playButton.png"),
    picturePauseButton(":/images/pauseButton.png")
{
}
/**
 * Change l'état du bouton (play/pause)
 *
 * @param playing
 *            Etat du bouton.
 */
void Playbutton::setState(bool playing) { isPlaying = playing; }

/**
 * Change l'état du bouton (play/pause)
 *
 * @param playing
 *            Etat du bouton.
 */
void Playbutton::playOrPause(bool playing) {
    setState(playing);
    update(); // redessine le bouton en fonction de isPlaying dans paintEvent
}

/**
 * Redessine l'image de fond du bouton (play/pause)
 *
 * @param type QPaintEvent *
 *            Evenement QPaint.
 */
void Playbutton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    if (isPlaying)
        painter.drawPixmap(0,0,picturePauseButton);
    else
        painter.drawPixmap(0,0,picturePlayButton);
}

/**
 * Change l'état du bouton lors d'un évènement souris (play/pause)
 *
 * @param event
 *            Evenement QMouse.
 */
void Playbutton::mousePressEvent(QMouseEvent *event) {
    if (isPlaying)
        setState(false);
    else
        setState(true);
    update();
}

