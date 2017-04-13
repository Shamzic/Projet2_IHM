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
    m_running=true;
    m_clientLoopThread=QtConcurrent::run(this, &Client::serverMessageLoop);
}

Client::~Client() {
    m_socket->disconnectFromServer();
    m_running=false;
    m_clientLoopThread.waitForFinished();
}


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
