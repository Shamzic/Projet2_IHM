/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef SERVEUR_H
#define SERVEUR_H

#include <QObject>
#include <QThread>
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
    void clientMessageLoop();

private:
    QLocalServer *m_server;
    QVector<QLocalSocket *> m_clients;
    QLocalSocket *mpv=NULL;
    void sendRequestToMPV(QJsonObject jsonObject);
    void sendMessageToClients(QJsonObject jsonObject);

private slots:
    void connectionFromClient();
    void clientDisconnected();
    void endServer();
    void readyRead();

public slots:
    // Messages reçus de l'automate
    void message(signalType,QVariantMap);

signals:
    //transmettre des commandes vers l'automate
    void signalFromServer(signalType, QVariantMap);
    void terminateSig();

};

#endif // SERVEUR_H
