/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#include "mainwindow.h"
#include "client.h"
#include <iostream>
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    int r;

    try {

       // QTranslator translator;
       // translator.load("../Client/client_en.qm"); // A voir  sur MAC
        Client * c = new Client();
        QApplication a(argc, argv);
       // a.installTranslator(&translator);
        MainWindow w;
        w.show();
        QObject::connect(&w, SIGNAL(signalUI(signalType, QVariantMap)),
                            c, SLOT(messageFromUI(signalType, QVariantMap)));
        qDebug()<<"YOLO ce truc s'affiche à la fin mais cela n'a aucun sens monolithe parallépipédique";
        QObject::connect(c, SIGNAL(signalFromClient(signalType, QVariantMap)),
                                    &w, SLOT(message(signalType, QVariantMap)));
        QObject::connect(&w,SIGNAL(terminatingApp()),c,SLOT(terminate()));

        r = a.exec();
    } catch (QString err) {
        std::cout << err.toStdString() << "\n";
        std::cout << "terminating" << "\n";
        r = 1;
    }
    return r;
}
