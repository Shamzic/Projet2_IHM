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

Automate::Automate(QObject *parent) : QObject(parent)
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
    stateFin = new QFinalState(Lecteur);

    // play vers fin, si fin du morceau
    statePlay->addTransition(TpsLecture,SIGNAL(timeout()),stateFin);
    // play vers pause, si signal pause
    statePlay->addTransition(this,SIGNAL(signalPause()),statePause);
    // pause vers play, si signal play
    statePause->addTransition(this,SIGNAL(signalPlay()),statePlay);

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
            setPlay(true);
            break;
        case kSignalPause:
            setPause(true);
            break;
        case kSignalEnd:
            cleanup();
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
}

void Automate::cleanup() {
    qDebug() << "cleanup" ;
    TpsLecture->stop();
    Lecteur->stop();
}


void Automate::setPlay(bool){
    Lecteur->setInitialState(statePlay);
    Lecteur->start();
    qDebug() << "debut machine" ;
}

void Automate::setPause(bool){
    emit signalPause();
}
