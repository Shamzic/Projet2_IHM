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
    connect(mpv, SIGNAL(disconnected()), this, SLOT(endServer()));

    m_running=true;
    m_serverLoopThread = QtConcurrent::run(this,&Serveur::clientMessageLoop);
   // m_mpvEventLoopThread = QtConcurrent::run(this,&Serveur::MPVEventMessageLoop);
}

Serveur::~Serveur() {
    qDebug() << "del server" ;
    mpv->disconnectFromServer();
    if(m_client)m_client->abort();
    m_server->close();
    m_running = false;
    m_serverLoopThread.waitForFinished();
    //m_mpvEventLoopThread.waitForFinished();
}


//mpv n'est plus accessible
void Serveur::endServer() {
    emit terminateSig();
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
        QString str=QString(in.device()->readLine());
        QByteArray a=str.toUtf8();
        QJsonParseError error;
        QJsonDocument jDoc=QJsonDocument::fromJson(a, &error);
        QJsonObject jsonObject=jDoc.object();

        qDebug() << "msg reçu";

        emit signalFromServer((signalType)jsonObject[kJsonSignal].toInt(),
        jsonObject[kJsonParams].toObject().toVariantMap());
    }
}

/*
void Serveur::MPVEventMessageLoop() {
    while (m_running) {
        QDataStream in(mpv);
        if ( in.atEnd() ) { // Rien dans la file d'attente
            QThread::msleep(100); // On attend 1/10s et on continue
            continue;
        }
        QString str=QString(in.device()->readLine());
        QByteArray a=str.toUtf8();
        QJsonParseError error;
        QJsonDocument jDoc=QJsonDocument::fromJson(a, &error);
        QJsonObject jsonObject=jDoc.object();

        qDebug() << "event message MPV reçu :";
        QVariantMap json_map = jsonObject.toVariantMap();
        qDebug() << json_map["event"].toString() ;
    }
}
*/


void Serveur::sendRequestToMPV(QJsonObject jsonObject) {
    QByteArray bytes = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)+"\n";
    if (mpv!=NULL) {
      mpv->write(bytes.data(), bytes.length());
      mpv->flush();
    }
}

/*
    QDataStream in(mpv);
    if (mpv->waitForReadyRead(300)) {
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
*/

void Serveur::sendMessageToClients(QJsonObject jsonObject) {
    QByteArray bytes = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)+"\n";
    if (m_client!=NULL) {
        m_client->write(bytes.data(), bytes.length());
        m_client->flush();
    }
}

//message de l'automate
void Serveur::message(signalType sig,QVariantMap varmap) {
    QJsonObject jsonObject ;
    QJsonObject jsonObjectClient ;
    QJsonArray a ;
    switch (sig) {
        case kSignalPlay:
            a.append("loadfile");
            a.append(varmap[kParamPath].toString());
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            break;
        case kSignalPause:
            a.append("set_property");
            a.append("pause");
            a.append(true);
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            break;
        case kSignalEndPause:
            a.append("set_property");
            a.append("pause");
            a.append(false);
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            break;
        case kSignalVolume:
            a.append("set_property");
            a.append("volume");
            a.append(varmap[kParamVolume].toInt());
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            jsonObjectClient[kJsonSignal]=sig;
            jsonObjectClient[kJsonParams]=QJsonObject::fromVariantMap(varmap);
            sendMessageToClients(jsonObjectClient);
            break;
        case kSignalEnd:
            break;
        default:
            break;
    }
}

