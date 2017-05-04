/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTOMATE_H
#define AUTOMATE_H

#include "../constantes.h"
#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QHistoryState>
#include <QFinalState>
#include <QSignalTransition>
#include <QTimer>
#include <QDebug>

class Automate : public QObject
{
    Q_OBJECT
public:
    explicit Automate(QObject *parent = 0);

private:
    QTimer *TpsLecture;
    QStateMachine *Lecteur;
    QState *stateAttenteAudio;
    QState *statePlay;
    QState *statePause;
    QState *stateReprendre;
    QFinalState *stateFin;
    QSignalTransition * transVolume;
    QSignalTransition * transMute;
    QSignalTransition * transUnmute;
    QSignalTransition * transTime;
    QString path;
    int state;
    int length = 0;
    int temps = 0;
    int volume = 100;
    int volumeHistory = 100;
    bool mute = false;
    void setupMessages();
    void cleanup();
    void addVolumeTransitions(QState * s);
    void removeVolumeTransitions(QState * s);

signals:
    // Messages de l'automate vers le serveur
    void signalLecteur(signalType, QVariantMap);
    void signalPlay();
    void signalPause();
    void signalFin();
    void signalNewAudio();
    void signalVolume();
    void signalMute();
    void signalUnmute();
    void signalTime();

public slots:
    // Messages du serveur vers l'automate
    void message(signalType, QVariantMap);
    void changeVolume();
    void changeToMute();
    void changeToUnmute();
    void changeTime();
};

#endif // AUTOMATE_H
