/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "automate.h"


/* Bon j'ai fais un peu yolo copié sur la machine...
 *
 * Mais en gros j'aimerais que on appuie sur play, ça lance la machine à état
 * et ça joue la liste en cours.
 * Si on fait pause et l'état du temps doit être sauvegardé.
 * Dès qu'une musique est finie, la suivante dans la liste est lancée.
 * Quand la liste est finie, la machine a état est terminée.
 *
 * */

 #include <QDebug>

Automate::Automate(QObject *parent) :
    QObject(parent),
    volume(100)
{
    // Timer durée des
    TpsLecture = new QTimer(this);
    TpsLecture->setSingleShot(true);
    TpsLecture->setTimerType(Qt::PreciseTimer);

    // Machine à états
    Lecteur = new QStateMachine(this);

    // Etats
    stateAttenteAudio = new QState(Lecteur);
    statePlay = new QState(Lecteur);
    statePause = new QState(Lecteur);
    stateReprendre = new QState(Lecteur);
    stateFin = new QFinalState(Lecteur);

    transVolume = new QSignalTransition(this,SIGNAL(signalVolume()));
    QObject::connect(transVolume, SIGNAL(triggered()),SLOT(changeVolume()));

    // attenteAudio vers play, si nouveau morceau
    stateAttenteAudio->addTransition(this,SIGNAL(signalNewAudio()),statePlay);
    // si tout est terminé, on arrête la machine
    stateAttenteAudio->addTransition(this,SIGNAL(signalFin()),stateFin);

    // play vers attenteAudio, si fin du morceau
    statePlay->addTransition(TpsLecture,SIGNAL(timeout()),stateAttenteAudio);
    // play vers pause, si signal pause
    statePlay->addTransition(this,SIGNAL(signalPause()),statePause);
    statePlay->addTransition(this,SIGNAL(signalNewAudio()),statePlay);

    // pause vers reprendre, si signal play
    statePause->addTransition(this,SIGNAL(signalPlay()),stateReprendre);
    statePause->addTransition(this,SIGNAL(signalNewAudio()),statePlay);

    // reprendre vers pause, si signal pause
    stateReprendre->addTransition(this,SIGNAL(signalPause()),statePause);
    // reprendre vers attenteAudio, si timeout
    stateReprendre->addTransition(TpsLecture,SIGNAL(timeout()),stateAttenteAudio);
    stateReprendre->addTransition(this,SIGNAL(signalNewAudio()),statePlay);


    QObject::connect(stateFin, &QState::entered, [this](){
      qDebug()<<"Fini";
      //QVariantMap params;
      //params[kParamPhase]=QVariant(kPhaseEndCycle);
      //emit signalMachine(kSignalPhase, params);
      cleanup();
    });

    Lecteur->setInitialState(stateAttenteAudio);
    Lecteur->start();

    setupMessages();
}

// messages reçu depuis le serveur
void Automate::message(signalType sig, QVariantMap params) {
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
            length = 20;
            emit signalNewAudio();
            break;
        case kSignalVolume:
            volume = params[kParamVolume].toInt();
            emit signalVolume();
            break;
        case kSignalMute:
            volumeHistory = volume;
            volume = 0;
            emit signalVolume();
            break;
        case kSignalUnmute:
            volume = volumeHistory;
            emit signalVolume();
            break;
        default:
            break;
    }
}

/* Messages vers le serveur */
void Automate::setupMessages() {
    QObject::connect(stateAttenteAudio, &QState::entered, [this](){
        stateAttenteAudio->addTransition(transVolume);
        qDebug()<<"entree attente audio";
    });

    QObject::connect(stateAttenteAudio, &QState::exited, [this](){
        stateAttenteAudio->removeTransition(transVolume);
        qDebug()<<"sortie attente audio";
    });

    QObject::connect(statePlay, &QState::entered, [this](){
        statePlay->addTransition(transVolume);
        qDebug()<<"entree state play";
        QVariantMap params;
        params[kParamPath]=QVariant(path);
        emit signalLecteur(kSignalPlay, params);
    });

    QObject::connect(statePlay, &QState::exited, [this](){
        statePlay->removeTransition(transVolume);
        qDebug()<<"sortie state play";
    });

    QObject::connect(statePause, &QState::entered, [this](){
        statePause->addTransition(transVolume);
        qDebug()<<"entree state pause";
        QVariantMap params;
        emit signalLecteur(kSignalPause, params);
    });

    QObject::connect(statePause, &QState::exited, [this](){
        statePause->removeTransition(transVolume);
        qDebug()<<"sortie state pause";
        QVariantMap params;
        emit signalLecteur(kSignalEndPause,params);
    });

    QObject::connect(stateReprendre, &QState::entered, [this](){
        stateReprendre->addTransition(transVolume);
        qDebug()<<"entree state reprendre";
    });

    QObject::connect(stateReprendre, &QState::exited, [this](){
        stateReprendre->removeTransition(transVolume);
        qDebug()<<"sortie state reprendre";
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
