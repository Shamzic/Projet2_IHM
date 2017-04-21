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
    try {
        Serveur s;
        QApplication a(argc, argv);
        //Automate m;

        // On connect le serveur à l'Automate
        //  QObject::connect(&m, SIGNAL(signalMachine(signalType, QVariantMap)), &s, SLOT(message(signalType, QVariantMap)));
        //  QObject::connect(&s, SIGNAL(signalUI(signalType, QVariantMap)), &m, SLOT(message(signalType, QVariantMap)));

        r = a.exec();
    } catch (QString err) {
        std::cout << err.toStdString() << "\n";
        std::cout << "terminating" << "\n";
        r = 1;
    }

    return r;
}
