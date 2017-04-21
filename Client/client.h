/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QLocalSocket>
#include <QtConcurrent/QtConcurrent>
#include <QVariantMap>

#include <QDebug>

#define SERVER_NAME "/tmp/client-socket"

extern const char kJsonSignal[];
extern const char kJsonParams[];

extern const char kVitesseLecture[];
extern const char kParamSwitch[];

enum signalType {
    kSignalPlay,
    kSignalPause,
    kSignalEnd
};

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

};

#endif // CLIENT_H
