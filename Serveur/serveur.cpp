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

    /* on se connecte à mpv */
    mpv->connectToServer(MPV_SERVER_NAME,QIODevice::ReadWrite);
    if (mpv->waitForConnected())
        qDebug() << "connected to mpv";
    else {
        throw mpv->errorString();
    }

    /* on lance le serveur pour parler avec les clients */
    QString serverName(AUTOMATE_SERVER_NAME);
    QLocalServer::removeServer(serverName);
    if (!m_server->listen(serverName)) {
        throw m_server->errorString();
    }

    /* on a un signal si un client se connecte */
    connect(m_server, SIGNAL(newConnection()), this, SLOT(connectionFromClient()));
    /* on termine si MPV se casse */
    connect(mpv, SIGNAL(disconnected()), this, SLOT(endServer()));

    m_running=true;
}

Serveur::~Serveur() {
    qDebug() << "del server" ;
    mpv->disconnectFromServer();
    int i= 0;
    for (i=0; i < m_clients.size() ;i++) {
        m_clients.at(i)->abort();
    }
    m_server->close();
    m_running = false;
}


/** Terminer si mpv n'est plus accessible */
void Serveur::endServer() {
    QJsonObject jsonObjectClient ;
    jsonObjectClient[kJsonSignal]=kSignalEnd;
    sendMessageToClients(jsonObjectClient);
    emit terminateSig();
}

/** slot appelé lorsqu'un client se connecte. On le met dans la liste de clients.
 * On prépare les signaux lorsque le client se déconnecte, et lorsqu'il nous a
 * écrit un message dans la socket */
void Serveur::connectionFromClient() {
    QLocalSocket * nextClient = m_server->nextPendingConnection();
    m_clients.append(nextClient);
    connect(nextClient, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(nextClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    connect(nextClient, SIGNAL(disconnected()), nextClient, SLOT(deleteLater()));
    qDebug() << "connexion client" ;
    QVariantMap map;
    emit signalFromServer(kSignalGetProperties, map);
}

/*
void Serveur::getProperty(char * which) {
    QJsonObject jsonObject ;
    QJsonArray a ;
    a.append("get_property"); a.append(which);
    jsonObject["command"]=a;
    sendRequestToMPV(jsonObject);
    QDataStream in(mpv);
    if (mpv->waitForReadyRead(300)) {
        QString str=QString(in.device()->readLine());
        QByteArray an=str.toUtf8();
        QJsonParseError error;
        QJsonDocument jDoc=QJsonDocument::fromJson(an, &error);
        QJsonObject jsonObject2=jDoc.object();
        QVariantMap json_map = jsonObject2.toVariantMap();
        qDebug() << json_map["data"].toString() ;
    }
}
*/

/** Ce slot est appelé lorsqu'un client se déconnecte. On l'enlève de la liste. */
void Serveur::clientDisconnected() {
    QLocalSocket* obj = qobject_cast<QLocalSocket *>(sender());
    m_clients.removeOne(obj);
    qDebug() << "déconnexion client" ;
}

/** Slot appelé quand un client a écrit un message */
void Serveur::readyRead() {
    QLocalSocket* client = qobject_cast<QLocalSocket *>(sender());
    if (m_running) {
        QDataStream in(client);
        if ( !in.atEnd() ) {
            QString str=QString(in.device()->readLine());
            QByteArray a=str.toUtf8();
            QJsonParseError error;
            QJsonDocument jDoc=QJsonDocument::fromJson(a, &error);
            QJsonObject jsonObject=jDoc.object();
            emit signalFromServer((signalType)jsonObject[kJsonSignal].toInt(),
            jsonObject[kJsonParams].toObject().toVariantMap());
        }
    }
}

void Serveur::sendRequestToMPV(QJsonObject jsonObject) {
    QByteArray bytes = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)+"\n";
    if (mpv!=NULL) {
      mpv->write(bytes.data(), bytes.length());
      mpv->flush();
    }
}

/** Envoyer un message à tous les clients */
void Serveur::sendMessageToClients(QJsonObject jsonObject) {
    QByteArray bytes = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)+"\n";
    int i=0;
     for (i=0; i < m_clients.size() ; i++) {
        m_clients.at(i)->write(bytes.data(), bytes.length());
        m_clients.at(i)->flush();
    }
}

/** Traiter un message de l'automate, càd préparer un message vers MPV et
    les clients */
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
            jsonObjectClient[kJsonSignal]=sig;
            jsonObjectClient[kJsonParams]=QJsonObject::fromVariantMap(varmap);
            sendMessageToClients(jsonObjectClient);
            break;
        case kSignalPause:
            a.append("set_property");
            a.append("pause");
            a.append(true);
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            jsonObjectClient[kJsonSignal]=sig;
            sendMessageToClients(jsonObjectClient);
            break;
        case kSignalEndPause:
            a.append("set_property");
            a.append("pause");
            a.append(false);
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            jsonObjectClient[kJsonSignal]=sig;
            sendMessageToClients(jsonObjectClient);
            break;
        case kSignalVolume:
        case kSignalMute:
        case kSignalUnmute:
            a.append("set_property");
            a.append("volume");
            a.append(varmap[kParamVolume].toInt());
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            jsonObjectClient[kJsonSignal]=sig;
            jsonObjectClient[kJsonParams]=QJsonObject::fromVariantMap(varmap);
            sendMessageToClients(jsonObjectClient);
            break;
        case kSignalTime:
            a.append("set_property");
            a.append("time-pos");
            a.append(varmap[kParamTime].toInt());
            jsonObject["command"]=a;
            sendRequestToMPV(jsonObject);
            jsonObjectClient[kJsonSignal]=sig;
            jsonObjectClient[kJsonParams]=QJsonObject::fromVariantMap(varmap);
            sendMessageToClients(jsonObjectClient);
            break;
        case kSignalGetProperties:
            jsonObjectClient[kJsonSignal]=kSignalGetProperties;
            jsonObjectClient[kJsonParams]=QJsonObject::fromVariantMap(varmap);
            sendMessageToClients(jsonObjectClient);
            break;
        case kSignalEnd:
            break;
        default:
            break;
    }
}

