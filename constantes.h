/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CONSTANTES_H
#define CONSTANTES_H

#define MPV_SERVER_NAME "/tmp/mpv-socket"
#define AUTOMATE_SERVER_NAME "/tmp/client-socket"

const char kVitesseLecture[]="vitesse";
const char kParamSwitch[]="switch";

const char kJsonSignal[]="signal";
const char kJsonParams[]="params";

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

const char * audio[]= {
    "Audio/Miley_Cyrus-Jolene.mp3",
    "Audio/Moana - I Am Moana.mp3"
};

#endif // CONSTANTES_H
