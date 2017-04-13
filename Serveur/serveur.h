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

class Serveur: public QObject
{
    Q_OBJECT
public:
    explicit Serveur(QObject *parent = 0);
    ~Serveur();
    void clientMessageLoop();
    bool m_running;
    QFuture<void> m_serverLoopThread;

private:
    QLocalServer *m_server;
    QLocalSocket *m_client=NULL;

public slots:
    // Messages reçus de l'automate
    void message(signalType,bool,QVariant,QVariant);

signals:
    //transmettre des commandes vers l'automate
    void signalFromServer(signalType, QVariantMap);
};

#endif // SERVEUR_H