/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "taglib/tag.h"
#include "taglib/fileref.h"
#include "taglib/tstring.h"
#include <taglib/tag.h>


#include <QDir>

using namespace TagLib;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mute(false),
    muteSymbol(":/images/audio_off.png"),
    volumeOnSymbol(":/images/audio_on.png")
{
    ui->setupUi(this);
    this->setWindowTitle("Audio Player");

    int i;
    for (i=0; audio_files[i] != nullptr; i++) {

#ifdef Q_OS_MAC
        QString filepath = "../../../../";
#else
        QString filepath = "../";
#endif

        filepath.append(QString(audio_files[i]));
        TagLib::FileRef f(filepath.toStdString().c_str());
        if(!f.isNull() && f.tag()) {
          //  TagLib::Tag *tag = f.tag();
           // qDebug() << tag->artist().toCString();
            QString description = f.tag()->artist().toCString();
            description.append(" - ");
            description.append(QString(f.tag()->title().toCString()));
            QTreeWidgetItem *audio = new QTreeWidgetItem(ui->AudioTree);
            //audio->setText(0, tr(audio_files[i]));
            audio->setText(0,description);
            audio->setData(0,Qt::UserRole,QVariant(audio_files[i]));
        }
    }

    for (i=0; radio_links[i] != nullptr; i++) {
        ui->RadioList->addItem(radio_links[i]);
    }

    connect(ui->playbutton,SIGNAL(changedState(bool)),this,SLOT(playbuttonClicked(bool)));
    connect(ui->AudioTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                        this,SLOT(audioDoubleClicked(QTreeWidgetItem *,int)));
    connect(this,SIGNAL(changeButtonState(bool)),ui->playbutton,SLOT(playOrPause(bool)));
    connect(ui->volumeBar,SIGNAL(volumeChanged(int)),this,SLOT(volumeBarClicked(int)));
    connect(this,SIGNAL(changeVolumeBar(int)),ui->volumeBar,SLOT(changeVolume(int)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *) {
    emit terminatingApp();
    QApplication::quit();
}

// envoyer message socket
// varmap ??
void MainWindow::playbuttonClicked(bool isPlaying) {
    QVariantMap varmap;
    if (isPlaying && ui->AudioTree->selectedItems().size() == 0){
        ui->AudioTree->setCurrentItem(ui->AudioTree->currentItem());
        varmap[kParamPath] = ui->AudioTree->currentItem()->data(0,Qt::UserRole);
        emit signalUI(kSignalChangeAudio,varmap);
    } else {
        varmap[kParamPath] = ui->AudioTree->currentItem()->data(0,Qt::UserRole);
        if (isPlaying) {
            emit signalUI(kSignalPlay,varmap);
        } else {
            emit signalUI(kSignalPause,varmap);
        }
    }
}

void MainWindow::volumeBarClicked(int v) {
    if (v==0) {
        on_muteButton_clicked();
    } else if (mute && v > 0 ){
        on_muteButton_clicked();
    }
    QVariantMap varmap;
    varmap[kParamVolume] = v;
    emit signalUI(kSignalVolume,varmap);
}

/** Ajoute un morceau audio au début de l'historique. Si le morceau y était
déjà, on enlève l'ancienne entrée dans la liste. */
void MainWindow::ajouterAHistorique(QString path) {
    QString description;
#ifdef Q_OS_MAC
    QString filepath = "../../../../";
#else
    QString filepath = "../";
#endif
    filepath.append(path);
    TagLib::FileRef f(filepath.toStdString().c_str());
    if(!f.isNull() && f.tag()) {
        description = f.tag()->artist().toCString();
        description.append(" - ");
        description.append(QString(f.tag()->title().toCString()));
    }
    qDebug() << description;
    QList<QListWidgetItem *> previous =
                    ui->HistoriqueList->findItems(description,Qt::MatchExactly);
    if (previous.size() != 0) {
        delete previous.at(0);
    }
    ui->HistoriqueList->insertItem(0,description);
}

// message du serveur
void MainWindow::message(signalType sig, QVariantMap params) {
    QString path;
    switch(sig) {
        case kSignalPlay:
            path = params[kParamPath].toString();
            ajouterAHistorique(path);
            break;
        case kSignalVolume:
        case kSignalMute:
        case kSignalUnmute:
            emit changeVolumeBar(params[kParamVolume].toInt());
            break;
        case kSignalGetProperties:
            path= params[kParamPath].toString();
            params[kParamVolume].toInt();
            if (path!="") {
                emit changeButtonState(true);
                qDebug() << path;
            }
            emit changeVolumeBar(params[kParamVolume].toInt());
            break;
        default:
            break;
    }
}

void MainWindow::audioDoubleClicked(QTreeWidgetItem *item, int column) {
    QVariantMap varmap;
    varmap[kParamPath] = item->data(column,Qt::UserRole);
    emit changeButtonState(true);
    emit signalUI(kSignalChangeAudio,varmap);
}

void MainWindow::on_muteButton_clicked() {
    QVariantMap varmap;
    if (mute) {
        mute = false;
        ui->muteButton->setIcon(QIcon(volumeOnSymbol));
        //emit changeVolumeBar(0);
        emit signalUI(kSignalUnmute,varmap);
    } else {
        mute = true;
        ui->muteButton->setIcon(QIcon(muteSymbol));
        emit changeVolumeBar(0);
        emit signalUI(kSignalMute,varmap);
    }
}
