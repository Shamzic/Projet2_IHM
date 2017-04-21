/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "serveur.h"

#include <QDebug>

const char kJsonSignal[]="signal";
const char kJsonParams[]="params";

Serveur::Serveur(QObject *parent) :
    QObject(parent),
    m_server(new QLocalServer(this)),
    m_client(new QLocalSocket(this)),
    mpv(new QLocalSocket(this))
{
    qRegisterMetaType<signalType>("signalType");
    m_running=true;
    m_serverLoopThread = QtConcurrent::run(this,&Serveur::clientMessageLoop);

    mpv->connectToServer(SERVER_NAME,QIODevice::ReadWrite);
    if (mpv->waitForConnected())
        qDebug() << "connected to mpv";
    else {
        throw mpv->errorString();
    }

    sendRequestToMPV();
     sendRequestToMPV2();
}

Serveur::~Serveur() {
    mpv->disconnectFromServer();
    m_running = false;
    m_serverLoopThread.waitForFinished();
}

/*
void Serveur::connectionFromClient() {
    if(m_client) return; // Un seul client
    m_client= m_server->nextPendingConnection();
    connect(m_client, SIGNAL(disconnected()), m_client, SLOT(deleteLater()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
}

void Serveur::clientDisconnected(){
    m_client=NULL;
}
*/

/*?????*/
void Serveur::clientMessageLoop() {
    while (m_running){
        QDataStream in(mpv);
        if (in.atEnd()){ // Rien dans la file d'attente
            QThread::msleep(100); // On attend 1/10s et on continue
            continue;
        }
        QString str=QString(in.device()->readLine());
        QByteArray a=str.toUtf8();
        QJsonParseError error;
        QJsonDocument jDoc=QJsonDocument::fromJson(a, &error);
        QJsonObject jsonObject=jDoc.object();
        emit signalFromServer((signalType)jsonObject[kJsonSignal].toInt(),
        jsonObject[kJsonParams].toObject().toVariantMap());
    }
}

void Serveur::sendRequestToMPV(){
    QJsonObject jsonObject ;

QJsonArray a ;
a.append("set_property");
a.append("volume");
a.append(70);

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
        qDebug() << "ff"  ;
    }
}

void Serveur::sendRequestToMPV2(){
    QJsonObject jsonObject ;

QJsonArray a ;
a.append("get_property");
a.append("volume");

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
        qDebug() << json_map["data"].toString() ;
    } else {
        qDebug() << "ff"  ;
    }
}


//message de l'automate
void Serveur::message(signalType,QVariantMap) {

}

