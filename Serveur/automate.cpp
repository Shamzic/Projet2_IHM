/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "automate.h"
#include <QDebug>

Automate::Automate(QObject *parent) :
    QObject(parent),
    temps(0),
    volume(100)
{
    // Timer durée des
    TpsLecture = new QTimer(this);
    TpsLecture->setSingleShot(true);
    TpsLecture->stop();
    TpsLecture->setTimerType(Qt::PreciseTimer);

    // Machine à états
    Lecteur = new QStateMachine(this);

    // Etats
    stateAttenteAudio = new QState(Lecteur);
    statePlay = new QState(Lecteur);
    statePause = new QState(Lecteur);
    stateReprendre = new QState(Lecteur);
    stateFin = new QFinalState(Lecteur);

    stateAttenteAudio->assignProperty(Lecteur,kParamEtat,QVariant(kStateAttente));
    statePlay->assignProperty(Lecteur,kParamEtat,QVariant(kStatePlay));
    statePause->assignProperty(Lecteur,kParamEtat,QVariant(kStatePause));
    stateReprendre->assignProperty(Lecteur,kParamEtat,QVariant(kStateReprendre));

    transVolume = new QSignalTransition(this,SIGNAL(signalVolume()));
    QObject::connect(transVolume, SIGNAL(triggered()),SLOT(changeVolume()));
    transMute = new QSignalTransition(this,SIGNAL(signalMute()));
    QObject::connect(transMute, SIGNAL(triggered()),SLOT(changeToMute()));
    transUnmute = new QSignalTransition(this,SIGNAL(signalUnmute()));
    QObject::connect(transUnmute, SIGNAL(triggered()),SLOT(changeToUnmute()));

    transTime = new QSignalTransition(this,SIGNAL(signalTime()));
    QObject::connect(transTime,SIGNAL(triggered()),SLOT(changeTime()));

    // attenteAudio vers play, si nouveau morceau
    stateAttenteAudio->addTransition(this,SIGNAL(signalPlay()),statePlay);
    stateAttenteAudio->addTransition(this,SIGNAL(signalNewAudio()),statePlay);
    // si tout est terminé, on arrête la machine
    stateAttenteAudio->addTransition(this,SIGNAL(signalFin()),stateFin);

    // play vers attenteAudio, si fin du morceau
    //statePlay->addTransition(TpsLecture,SIGNAL(timeout()),stateAttenteAudio);
    // play vers pause, si signal pause
    statePlay->addTransition(this,SIGNAL(signalPause()),statePause);
    statePlay->addTransition(this,SIGNAL(signalNewAudio()),statePlay);

    // pause vers reprendre, si signal play
    statePause->addTransition(this,SIGNAL(signalPlay()),stateReprendre);
    statePause->addTransition(this,SIGNAL(signalNewAudio()),statePlay);

    // reprendre vers pause, si signal pause
    stateReprendre->addTransition(this,SIGNAL(signalPause()),statePause);
    // reprendre vers attenteAudio, si timeout
    //stateReprendre->addTransition(TpsLecture,SIGNAL(timeout()),stateAttenteAudio);
    stateReprendre->addTransition(this,SIGNAL(signalNewAudio()),statePlay);

    QObject::connect(stateReprendre, SIGNAL(entered()), TpsLecture, SLOT(start()));
    QObject::connect(statePlay, SIGNAL(entered()), TpsLecture, SLOT(start()));
    QObject::connect(stateAttenteAudio, SIGNAL(entered()), TpsLecture, SLOT(stop()));
    QObject::connect(statePause, SIGNAL(entered()), TpsLecture, SLOT(stop()));

    QObject::connect(stateFin, &QState::entered, [this](){
      qDebug()<<"Fini";
      cleanup();
    });

    Lecteur->setInitialState(stateAttenteAudio);
    Lecteur->start();
    setupMessages();

    connect(TpsLecture,SIGNAL(timeout()),this,SLOT(putain()));
}

void Automate::putain() {
    qDebug() << "timeout";
}

// messages reçu depuis le serveur
void Automate::message(signalType sig, QVariantMap params) {
    QVariantMap properties;
    int restant;
    switch(sig){
        case kSignalPlay:
            emit signalPlay();
            break;
        case kSignalPause:
            emit signalPause();
            break;
        case kSignalEnd:
            emit signalFin();
            break;
        case kSignalChangeAudio:
            path = params[kParamPath].toString();
            length = params[kParamLength].toInt();
            TpsLecture->setInterval(length);
            qDebug() << "timer changé";
            emit signalNewAudio();
            break;
        case kSignalVolume:
            volume = params[kParamVolume].toInt();
            emit signalVolume();
            break;
        case kSignalMute:
            volumeHistory = volume;
            volume = 0;
            mute = true;
            emit signalMute();
            break;
        case kSignalUnmute:
            volume = volumeHistory;
            mute = false;
            emit signalUnmute();
            break;
        case kSignalTime:
            temps = params[kParamTime].toInt();
            restant = length-temps;
            TpsLecture->setInterval(restant);
            qDebug() << "timer changé";
            emit signalTime();
        case kSignalGetProperties:
            properties[kParamVolume] = QVariant(volume);
            properties[kParamPath] = QVariant(path);
            properties[kParamLength] = QVariant(length);
            properties[kParamTime] = QVariant(TpsLecture->remainingTime());
            properties[kParamEtat] = Lecteur->property(kParamEtat);
            emit signalLecteur(kSignalGetProperties,properties);
            break;
        default:
            break;
    }
}

void Automate::addVolumeTransitions(QState * s) {
    s->addTransition(transVolume);
    s->addTransition(transMute);
    s->addTransition(transUnmute);
    s->addTransition(transTime);
}

void Automate::removeVolumeTransitions(QState * s) {
    s->removeTransition(transVolume);
    s->removeTransition(transMute);
    s->removeTransition(transUnmute);
    s->removeTransition(transTime);
}

/* Messages vers le serveur */
void Automate::setupMessages() {
    QObject::connect(stateAttenteAudio, &QState::entered, [this](){
        addVolumeTransitions(stateAttenteAudio);
        qDebug()<<"entree attente audio";
        qDebug() << TpsLecture->remainingTime();
        qDebug() << TpsLecture->isActive();
    });

    QObject::connect(stateAttenteAudio, &QState::exited, [this](){
        removeVolumeTransitions(stateAttenteAudio);
    });

    QObject::connect(statePlay, &QState::entered, [this](){
    qDebug() << "duree :" << TpsLecture->interval();
        addVolumeTransitions(statePlay);
        qDebug()<<"entree state play";
        QVariantMap params;
        params[kParamPath]=QVariant(path);
        emit signalLecteur(kSignalPlay, params);
    });

    QObject::connect(statePlay, &QState::exited, [this](){
        removeVolumeTransitions(statePlay);
    });

    QObject::connect(statePause, &QState::entered, [this](){
        addVolumeTransitions(statePause);
        qDebug()<<"entree state pause";
        QVariantMap params;
        emit signalLecteur(kSignalPause, params);
    });

    QObject::connect(statePause, &QState::exited, [this](){
        removeVolumeTransitions(statePause);
        QVariantMap params;
        emit signalLecteur(kSignalEndPause,params);
    });

    QObject::connect(stateReprendre, &QState::entered, [this](){
        addVolumeTransitions(stateReprendre);
        qDebug()<<"entree state reprendre";
    });

    QObject::connect(stateReprendre, &QState::exited, [this](){
        removeVolumeTransitions(stateReprendre);
    });
}

void Automate::cleanup() {
    qDebug() << "cleanup" ;
    TpsLecture->stop();
    Lecteur->stop();
}

void Automate::changeVolume() {
    qDebug() << "changement volume";
    QVariantMap params;
    params[kParamVolume] = QVariant(volume);
    emit signalLecteur(kSignalVolume, params);
}

void Automate::changeToMute() {
    qDebug() << "automate: mute";
    QVariantMap params;
    params[kParamVolume] = QVariant(volume);
    emit signalLecteur(kSignalMute, params);
}

void Automate::changeToUnmute() {
    qDebug() << "automate: unmute";
    QVariantMap params;
    params[kParamVolume] = QVariant(volumeHistory);
    emit signalLecteur(kSignalUnmute, params);
}

void Automate::changeTime() {
    qDebug() << "changement temps";
    QVariantMap params;
    params[kParamTime] = QVariant(temps);
    emit signalLecteur(kSignalTime, params);
}
