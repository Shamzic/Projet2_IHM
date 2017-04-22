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
    QState *statePlay;
    QState *statePause;
    QFinalState *stateFin;
    QString path;
    int length = 0;
    void setupMessages();
    void cleanup();

signals:
    // Messages de l'automate vers le serveur
    void signalLecteur(signalType, QVariantMap);
    void signalPlay();
    void signalPause();

private slots:
    void setPlay(bool);
    void setPause(bool);

public slots:
    // Messages du serveur vers l'automate
    void message(signalType, QVariantMap);
};

#endif // AUTOMATE_H
