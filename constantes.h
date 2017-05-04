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
const char kParamPath[]="path";
const char kParamVolume[] = "volume";
const char kParamTime[] = "time";
const char kParamLength[] = "duree";
const char kParamEtat[] = "etat";

const char kTimerInterval[]="interval";

const char kJsonSignal[]="signal";
const char kJsonParams[]="params";

enum signalType {
    kSignalPlay,
    kSignalPause,
    kSignalEndPause,
    kSignalEnd,
    kSignalTerminate,
    kSignalChangeAudio,
    kSignalVolume,
    kSignalMute,
    kSignalUnmute,
    kSignalTime,
    kSignalGetProperties
};

// Vitesses de lecture
enum speed {
    kVitesseNormale=0,
    kVitesseDouble=1
};

enum states {
    kStatePause,
    kStatePlay,
    kStateAttente,
    kStateReprendre
};

const char * const audio_files[]= {
    "Audio/Miley_Cyrus-Jolene.mp3",
    "Audio/Moana - I Am Moana.mp3",
    "Audio/EDEN - rock and roll.mp3",
    "Audio/Ludovico Einaudi - Fly.mp3",
    "Audio/Sing - I'm Still Standing.mp3",
    "Audio/10 Unknown Soldier.mp3",
    "Audio/Never Give Up.mp3",
    nullptr
};

const char * const radio_links[]= {
    "http://uk1.internet-radio.com:8294/live.m3u",
    "http://us1.internet-radio.com:8533/listen.pls",
    nullptr
};

#endif // CONSTANTES_H
