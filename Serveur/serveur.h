/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef SERVEUR_H
#define SERVEUR_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QtConcurrent/QtConcurrent>
#include <QVector>
#include "automate.h"

#define MPV_SERVER_NAME "/tmp/mpv-socket" //"\\\\.\\pipe\\mpv-pipe"
                                          //"/tmp/mpv-socket"
#define AUTOMATE_SERVER_NAME "/tmp/client-socket"

extern const char kJsonSignal[];
extern const char kJsonParams[];

class Serveur: public QObject
{
    Q_OBJECT
public:
    explicit Serveur(QObject *parent = 0);
    ~Serveur();
    bool m_running;
    QFuture<void> m_serverLoopThread;

private:
    QLocalServer *m_server;
    QLocalSocket * m_client;
    QLocalSocket *mpv=NULL;
    void clientMessageLoop();
    void sendRequestToMPV();

private slots:
    void connectionFromClient();
    void clientDisconnected();

public slots:
    // Messages reçus de l'automate
    void message(signalType,QVariantMap);

signals:
    //transmettre des commandes vers l'automate
    void signalFromServer(signalType, QVariantMap);

};

#endif // SERVEUR_H
