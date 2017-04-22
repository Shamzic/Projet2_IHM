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
    playing(false)
{
    // Timer durée des
    TpsLecture = new QTimer(this);
    TpsLecture->setSingleShot(true);
    TpsLecture->setTimerType(Qt::PreciseTimer);

    // Machine à états
    Lecteur = new QStateMachine(this);

    // Etats
    statePlay = new QState(Lecteur);
    statePause = new QState(Lecteur);
    stateReprendre = new QState(Lecteur);
    stateFin = new QFinalState(Lecteur);

    // play vers fin, si fin du morceau
    statePlay->addTransition(TpsLecture,SIGNAL(timeout()),stateFin);
    // play vers pause, si signal pause
    statePlay->addTransition(this,SIGNAL(signalPause()),statePause);
    // pause vers reprendre, si signal play
    statePause->addTransition(this,SIGNAL(signalPlay()),stateReprendre);
    // reprendre vers pause, si signal pause
    stateReprendre->addTransition(this,SIGNAL(signalPause()),statePause);
    // reprendre ver fin, si fin du morceau
    stateReprendre->addTransition(TpsLecture,SIGNAL(timeout()),stateFin);

    QObject::connect(stateFin, &QState::entered, [this](){
      qDebug()<<"Fini";
      //QVariantMap params;
      //params[kParamPhase]=QVariant(kPhaseEndCycle);
      //emit signalMachine(kSignalPhase, params);
      cleanup();
    });

    setupMessages();
}

// messages reçu depuis le serveur
void Automate::message(signalType sig, QVariantMap params) {
    switch(sig){
        case kSignalPlay:
            path = params["path"].toString();
            length = 20;
            setPlay();
            break;
        case kSignalPause:
            setPause();
            break;
        case kSignalEnd:
            cleanup();
            break;
        case kSignalChangeAudio:
            cleanup();
            path = params["path"].toString();
            length = 20;
            setPlay();
            break;
        default:
            break;
    }
}

/* Messages vers le serveur */
void Automate::setupMessages() {
    QObject::connect(statePlay, &QState::entered, [this](){
      qDebug()<<"entree state play";
      QVariantMap params;
      params[kParamPath]=QVariant(path);
      emit signalLecteur(kSignalPlay, params);
    });

    QObject::connect(statePlay, &QState::exited, [this](){
      qDebug()<<"sortie state play";
    });

    QObject::connect(statePause, &QState::entered, [this](){
      qDebug()<<"entree state pause";
      QVariantMap params;
      emit signalLecteur(kSignalPause, params);
    });

    QObject::connect(statePause, &QState::exited, [this](){
      qDebug()<<"sortie state pause";
      QVariantMap params;
      emit signalLecteur(kSignalEndPause,params);
    });

    QObject::connect(stateReprendre, &QState::entered, [this](){
      qDebug()<<"entree state reprendre";
    });

    QObject::connect(stateReprendre, &QState::exited, [this](){
      qDebug()<<"sortie state reprendre";
    });
}

void Automate::cleanup() {
    qDebug() << "cleanup" ;
    TpsLecture->stop();
    Lecteur->stop();
    playing = false;
}


void Automate::setPlay(){
    qDebug() << "ici";
    if (!playing) {
        Lecteur->setInitialState(statePlay);
        Lecteur->start();
        playing = true ;
        qDebug() << "debut machine" ;
    }
    emit signalPlay();
}

void Automate::setPause(){
    emit signalPause();
}
