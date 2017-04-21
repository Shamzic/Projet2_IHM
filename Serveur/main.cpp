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
    try {
        Serveur s;
    } catch (QString err) {
        std::cout << err.toStdString() << "\n";
        std::cout << "terminating" << "\n";
        return 1;
    }

    QApplication a(argc, argv);
    //Automate m;

    // On connect le serveur à l'Automate
    //  QObject::connect(&m, SIGNAL(signalMachine(signalType, QVariantMap)), &s, SLOT(message(signalType, QVariantMap)));
    //  QObject::connect(&s, SIGNAL(signalUI(signalType, QVariantMap)), &m, SLOT(message(signalType, QVariantMap)));

    return a.exec();
}
