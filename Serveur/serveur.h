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

class Serveur: public QObject
{
    Q_OBJECT
public:
    explicit Serveur(QObject *parent = 0);
    ~Serveur();
    bool m_running;
    QFuture<void> m_serverLoopThread;
    //QFuture<void> m_mpvEventLoopThread;
    void clientMessageLoop();
    //void MPVEventMessageLoop();

private:
    QLocalServer *m_server;
    QLocalSocket * m_client=NULL;
    QLocalSocket *mpv=NULL;
    void sendRequestToMPV(QJsonObject jsonObject);

private slots:
    void connectionFromClient();
    void clientDisconnected();
    void endServer();

public slots:
    // Messages reçus de l'automate
    void message(signalType,QVariantMap);

signals:
    //transmettre des commandes vers l'automate
    void signalFromServer(signalType, QVariantMap);
    void terminateSig();

};

#endif // SERVEUR_H
