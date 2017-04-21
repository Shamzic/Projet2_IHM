/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "serveur.h"

#include <QDebug>

Serveur::Serveur(QObject *parent) :
    QObject(parent),
    m_server(new QLocalServer(this)),
    mpv(new QLocalSocket(this))
{
    qRegisterMetaType<signalType>("signalType");

    mpv->connectToServer(MPV_SERVER_NAME,QIODevice::ReadWrite);
    if (mpv->waitForConnected())
        qDebug() << "connected to mpv";
    else {
        throw mpv->errorString();
    }

    // lancer le serveur pour parler avec les clients
    QString serverName(AUTOMATE_SERVER_NAME);
    QLocalServer::removeServer(serverName);
    if (!m_server->listen(serverName)) {
        throw m_server->errorString();
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(connectionFromClient()));

    m_running=true;
    m_serverLoopThread = QtConcurrent::run(this,&Serveur::clientMessageLoop);

    sendRequestToMPV();
}

Serveur::~Serveur() {
    qDebug() << "del server" ;
    mpv->disconnectFromServer();
    if(m_client)m_client->abort();
    m_server->close();
    m_running = false;
    m_serverLoopThread.waitForFinished();
}

// slot appelé lorsqu'un client se connecte
void Serveur::connectionFromClient() {
    if (m_client) return; // Un seul client pour l'instant...
    m_client= m_server->nextPendingConnection();
    connect(m_client, SIGNAL(disconnected()), m_client, SLOT(deleteLater()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    qDebug() << "connexion client" ;
}

// slot appelé lorsqu'un client se déconnecte
void Serveur::clientDisconnected() {
    qDebug() << "déconnexion client" ;
    m_client=NULL;
}


// lire les messages des clients
// on reçoit un truc du genre { "signal" = numero_signal ,
//    "params" = [<un variant map avec des parametres>] }
//jsonObject["signal"]=sig;
//jsonObject[kJsonParams]=QJsonObject::fromVariantMap(params);
void Serveur::clientMessageLoop() {
    while (m_running) {
        QDataStream in(m_client);
        if ( in.atEnd() ) { // Rien dans la file d'attente
            QThread::msleep(100); // On attend 1/10s et on continue
            continue;
        }
        qDebug() << "msg reçu";
        QString str=QString(in.device()->readLine());
        QByteArray a=str.toUtf8();
        QJsonParseError error;
        QJsonDocument jDoc=QJsonDocument::fromJson(a, &error);
        QJsonObject jsonObject=jDoc.object();

        qDebug() << jsonObject[kJsonSignal].toInt();

        emit signalFromServer((signalType)jsonObject[kJsonSignal].toInt(),
        jsonObject[kJsonParams].toObject().toVariantMap());
    }
}



void Serveur::sendRequestToMPV(){
    QJsonObject jsonObject ;


QJsonArray a ;
a.append("loadfile");
a.append(audio_files[0]);

    jsonObject["command"]=a;
    QByteArray bytes = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)+"\n";
    if (mpv!=NULL) {
      mpv->write(bytes.data(), bytes.length());
      mpv->flush();
    }

    QDataStream in(mpv);
    if (mpv->waitForReadyRead()) {
        QString str=QString(in.device()->readLine());
        QByteArray an=str.toUtf8();
        QJsonParseError error;
        QJsonDocument jDoc=QJsonDocument::fromJson(an, &error);
        QJsonObject jsonObject2=jDoc.object();
        QVariantMap json_map = jsonObject2.toVariantMap();
        qDebug() << json_map["error"].toString() ;
    } else {
        qDebug() << "fuck"  ;
    }
}


//message de l'automate
void Serveur::message(signalType,QVariantMap) {

}

