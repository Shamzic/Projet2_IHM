/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <QObject>
#include <QStateMachine>;
#include <QState>;
#include <QHistoryState>
#include <QFinalState>
#include <QSignalTransition>
#include <QTimer>
#include <QDebug>

enum signalType {
  kSignalPlay,
  kSignalPause,
  kSignalEnd
};

// Phase de lecture de liste
enum phaseliste{
    kPhaseLecture,
    kPhaseEnd
};

// Vitesses de lecture
enum speed {
  kVitesseStop=0,
  kVitesse1=1,
  kVitesse2=2,
  kVitesseBack1=-1,
  kVitesseBack2=-1
};



class automate : public QObject
{
    Q_OBJECT
public:
    explicit automate(QObject *parent = 0);

private:
    QTimer *TpsLecture;

    QStateMachine *Lecteur;

    QState *statePlay;
    QState *statePlayNext;
    QState *statePause;
    QState *stateAvanceRapide;
    QState *stateRetourRapide;

     QHistoryState *PlayHistory;
    // QHistoryState *AvanceRapideHistory;

    QSignalTransition *lecture_morceau;

    QFinalState *stateFin;

signals:

    void signalLecteur(signalType,bool on=true, int param1=0);

    void signalPlay();
    void signalPause();

public slots:
    void setPlay(bool);
    void setPause(bool);
    void setAcceleration(int factor);
};

#endif // AUTOMATE_H
