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

signals:

public slots:

protected:
    QPixmap picturePlayButton;
    QPixmap picturePauseButton;
    void paintEvent(QPaintEvent *e);
};

#endif // PLAYBUTTON_H
