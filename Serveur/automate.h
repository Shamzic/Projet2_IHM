/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <QObject>

class automate : public QObject
{
    Q_OBJECT
public:
    explicit automate(QObject *parent = 0);

signals:

public slots:
};

#endif // AUTOMATE_H