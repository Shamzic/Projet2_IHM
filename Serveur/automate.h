/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QHistoryState>
#include <QFinalState>
#include <QSignalTransition>
#include <QTimer>
#include <QDebug>

extern const char kVitesseLecture[];
extern const char kParamSwitch[];

enum signalType {
    kSignalPlay,
    kSignalPause,
    kSignalEnd
};

// Phase de lecture de liste
enum phaseliste{
    kPhaseLecture,
    kPhasePause,
    kPhaseEnd
};

// Vitesses de lecture
enum speed {
  kVitesseNormale=0,
  kVitesseDouble=1
};



class Automate : public QObject
{
    Q_OBJECT
public:
    explicit Automate(QObject *parent = 0);

private:
    QTimer *TpsLecture;
    QStateMachine *Lecteur;
    QState *statePlay;
    QState *statePause;
    QFinalState *stateFin;
    void setupMessages();
    void cleanup();

signals:
    // Messages vers l'UI
    void signalLecteur(signalType, QVariantMap);
    void signalPlay();
    void signalPause();

private slots:
    void setPlay(bool);
    void setPause(bool);

public slots:
    // Messages venant de l'UI
    void message(signalType, QVariantMap);
};

#endif // AUTOMATE_H
