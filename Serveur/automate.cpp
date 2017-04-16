/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#include "automate.h"

static const char kVitesseLecture[]="vitesse";
/* Bon j'ai fais un peu yolo copié sur la machine...
 *
 * Mais en gros j'aimerais que on appuie sur play, ça lance la machine à état
 * et ça joue la liste en cours.
 * Si on fait pause et l'état du temps doit être sauvegardé.
 * Dès qu'une musique est finie, la suivante dans la liste est lancée.
 * Quand la liste est finie, la machine a état est terminée.
 *
 * */

automate::automate(QObject *parent) : QObject(parent)
{
    // Timer durée des
    TpsLecture = new QTimer(this);
    TpsLecture->setSingleShot(true);
    TpsLecture->setTimerType(Qt::PreciseTimer);

    // Machine à états
    Lecteur = new QStateMachine(this);

    // Etats
    statePlay = new QState(Lecteur);
    statePlayNext = new QState(Lecteur);
    statePause = new QState(Lecteur);
    stateAvanceRapide = new QState(Lecteur);
    stateRetourRapide = new QState(Lecteur);
    stateFin = new QState(Lecteur);

    // Historiques ???
    PlayHistory = new QHistoryState(statePlay);
    // AvanceRapideHistory = new QHistoryState(stateAvanceRapide);

     // transitions
    // Fin de la lecture, play le morceau suivant
    lecture_morceau=statePlay->addTransition(TpsLecture,SIGNAL(timeout()),statePlayNext);

    QObject::connect(statePlayNext, SIGNAL(entered()),this,SLOT(start()));

    statePause->addTransition(this, SIGNAL(signalPause()),PlayHistory);
    // Si play => lis morceau selectionné dans la liste
    // Si pause => sauvegarde position et stop lecture
    // Si fin de pause => reprise de la lecture et de la liste
    // si fin lecture => lecture morceau suivant
    // Si fin liste => Stop

    // Si onglet historique
        // Affiche les derniers morceaux
        // si play, lis morceau selectionné et liste associée
    // Si onglet radio
        // Affiche les radios disponibles
        // si play radio => lis radio selectionnée

}

void automate::setupMessages(){
    QObject::connect(statePlay, &QState::entered, [this](){
        emit signalLecteur(kSignalPhase, true);
      });
}

void automate::start(bool on){
  if (!on) { // On arrête la machine
      TpsLecture->stop();
      Lecteur->stop();
      cleanup();
      return;
    }
}

void Automate::cleanup(){
  // On remet tout à zéro
}


void automate::setPlay(bool){
  emit signalPlay();
}

void automate::setPause(bool){
  emit signalPause();
}

void automate::setAcceleration(int factor) {
  accelerationFactor=factor;
}
