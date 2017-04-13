/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#ifndef PLAYBUTTON_H
#define PLAYBUTTON_H

#include <QWidget>
#include <QPainter>

class Playbutton: public QWidget
{
    Q_OBJECT
public:
    Playbutton(QWidget *parent = 0);

public slots:
    void playOrPause(bool playing);

signals:
    void changedState(bool playing);

protected:
    bool isPlaying;
    QPixmap picturePlayButton;
    QPixmap picturePauseButton;
    void setState(bool playing);
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *event);
};

#endif // PLAYBUTTON_H
