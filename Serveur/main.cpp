/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "serveur.h"
#include <iostream>
#include "automate.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    int r;
    QApplication a(argc, argv);
    try {
        Serveur s;
        Automate m;

        // On connect le serveur à l'Automate
        QObject::connect(&m, SIGNAL(signalLecteur(signalType, QVariantMap)),
                            &s, SLOT(message(signalType, QVariantMap)));
        QObject::connect(&s, SIGNAL(signalFromServer(signalType, QVariantMap)),
                            &m, SLOT(message(signalType, QVariantMap)));

        QObject::connect(&s,SIGNAL(terminateSig()),&a,SLOT(quit()));
        r = a.exec();
    } catch (QString err) {
        std::cout << err.toStdString() << "\n";
        std::cout << "terminating" << "\n";
        r = 1;
    }

    return r;
}
