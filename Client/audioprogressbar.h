/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef AUDIOPROGRESSBAR_H
#define AUDIOPROGRESSBAR_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QtMath>


class AudioProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit AudioProgressBar(QWidget *parent = 0);

signals:

public slots:
    void changeAudio(int length);

protected:
    int timePassed; // in ms ?
    int audioLength;
    QPixmap pictureFilledProgressBar;
    QPixmap pictureEmptyProgressBar;
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

};

#endif // AUDIOPROGRESSBAR_H
