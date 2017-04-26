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

const char kJsonSignal[]="signal";
const char kJsonParams[]="params";

enum signalType {
    kSignalPlay,
    kSignalPause,
    kSignalEndPause,
    kSignalEnd,
    kSignalTerminate,
    kSignalChangeAudio,
    kSignalVolume
};

// Vitesses de lecture
enum speed {
    kVitesseNormale=0,
    kVitesseDouble=1
};

const char * const audio_files[]= {
    "Audio/Miley_Cyrus-Jolene.mp3",
    "Audio/Moana - I Am Moana.mp3",
    "Audio/Ludovico Einaudi - Fly.mp3",
    nullptr
};

const char * const radio_links[]= {
    "http://uk1.internet-radio.com:8294/live.m3u",
    "http://us1.internet-radio.com:8533/listen.pls",
    nullptr
};

#endif // CONSTANTES_H
