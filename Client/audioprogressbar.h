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

class AudioProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit AudioProgressBar(QWidget *parent = 0);

signals:

public slots:

protected:
    QPixmap pictureFilledProgressBar;
    QPixmap pictureEmptyProgressBar;
    void paintEvent(QPaintEvent *e);

};

#endif // AUDIOPROGRESSBAR_H
