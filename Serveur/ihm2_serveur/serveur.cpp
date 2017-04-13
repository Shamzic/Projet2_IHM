/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "serveur.h"

Serveur::Serveur(QObject *parent) :
    QObject(parent),
    m_server(new QLocalServer(this))
{
    qRegisterMetaType<signalType>("signalType");
    m_running=true;
    m_serverLoopThread = QtConcurrent::run(this,&Serveur::clientMessageLoop);
}

void Serveur::~Serveur() {
    m_running = false;
    m_serverLoopThread.waitForFinished();
}


void Serveur::connectionFromClient(){
    if(m_client) return; // Un seul client
    m_client= m_server->nextPendingConnection();
    connect(m_client, SIGNAL(disconnected()), m_client, SLOT(deleteLater()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
}

void Serveur::clientDisconnected(){
    m_client=NULL;
}

void Serveur::clientMessageLoop() {
    while (m_running){
        QDataStream in(m_client);
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

//message de l'automate
void Serveur::message(signalType,bool,QVariant,QVariant) {
    QString serverName(SERVER_NAME);
    QLocalServer::removeServer(serverName);
    if (!m_server->listen(serverName)) {
        if(m_client)m_client->abort();
        m_server->close();
        return;
    }
    connect(m_server, SIGNAL(newConnection()), this, SLOT(connectionFromClient()));
}
