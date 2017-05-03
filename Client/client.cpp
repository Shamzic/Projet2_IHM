/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "client.h"

Client::Client(QObject *parent) :
    QObject(parent),
    m_socket(new QLocalSocket(this))
{
    qRegisterMetaType<signalType>("signalType");

    m_socket->connectToServer(AUTOMATE_SERVER_NAME);
    if (m_socket->waitForConnected()) {
         qDebug() << "connected to server";
    } else {
        throw m_socket->errorString();
    }

    m_running=true;
    m_clientLoopThread=QtConcurrent::run(this, &Client::serverMessageLoop);
}

Client::~Client() {
    m_socket->disconnectFromServer();
    m_running=false;
    m_clientLoopThread.waitForFinished();
}

// réception en boucle des messages du serveur
void Client::serverMessageLoop() {
    while (m_running){
        QDataStream in(m_socket);
        if (in.atEnd()){ // Rien dans la file d'attente
            QThread::msleep(100); // On attend 1/10s et on continue
            continue;
        }
        QString str=QString(in.device()->readLine());
        if(str=="") continue; // Evitons les lignes vides.
        QByteArray a=str.toUtf8();
        QJsonParseError error;
        QJsonDocument jDoc=QJsonDocument::fromJson(a, &error);
        QJsonObject jsonObject=jDoc.object();
        emit signalFromClient((signalType)jsonObject[kJsonSignal].toInt(),
        jsonObject[kJsonParams].toObject().toVariantMap());
   }
}

// envoyer un message au serveur
void Client::sendRequestToSocket(signalType sig, QVariantMap params) {
    QJsonObject jsonObject ;
    jsonObject[kJsonSignal]=sig;
    jsonObject[kJsonParams]=QJsonObject::fromVariantMap(params);

    QByteArray bytes = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact)+"\n";
    if (m_socket!=NULL) {
        m_socket->write(bytes.data(), bytes.length());
        m_socket->flush();
        qDebug() << "message sent to server";
    }
}

// message reçu de l'interface graphique -> notifier serveur
void Client::messageFromUI(signalType sig, QVariantMap param) {
    qDebug() << "received signal from UI";
    sendRequestToSocket(sig,param);
}
