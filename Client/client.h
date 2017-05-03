/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CLIENT_H
#define CLIENT_H

#include "../constantes.h"
#include <QObject>
#include <QLocalSocket>
#include <QtConcurrent/QtConcurrent>
#include <QVariantMap>

#include <QDebug>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    ~Client();

private:
    QLocalSocket *m_socket;
    void serverMessageLoop();
    bool m_running;
    QFuture<void> m_clientLoopThread;
    void sendRequestToSocket(signalType sig, QVariantMap params);

signals:
    //msg du client vers l'UI
    void signalFromClient(signalType, QVariantMap);

public slots:
    //msg de l'UI vers le client
    void messageFromUI(signalType, QVariantMap);
    void readyRead();
};

#endif // CLIENT_H
