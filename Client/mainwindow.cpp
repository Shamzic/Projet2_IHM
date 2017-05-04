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
    volumeOnSymbol(":/images/audio_on.png"),
    timer(new QTimer()),
    timer2(new QTimer()),
    secondes(0),
    minutes(0),
    duree(0)

{
    ui->setupUi(this);
    ui->retranslateUi(this);
    this->setWindowTitle(tr("Audio Player"));

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
            //qDebug()<<"durée du morceau :"<<QString::number(f.audioProperties()->lengthInSeconds());
            audio->setText(1,QString::number(f.audioProperties()->lengthInSeconds()));
            audio->setData(0,Qt::UserRole,QVariant(audio_files[i]));
            ui->AudioTree->setCurrentItem(audio->treeWidget()->topLevelItem(0));
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

    connect(ui->audioProgress,SIGNAL(timeChanged(int)),this,SLOT(audioProgressClicked(int)));
    connect(this,SIGNAL(changeTimeBar(int)),ui->audioProgress,SLOT(changeAudio(int)));
    connect(this,SIGNAL(changeMaxTimeBar(int)),ui->audioProgress,SLOT(changeMax(int)));
    connect(this,SIGNAL(addSecond()),ui->audioProgress,SLOT(addOneSecond()));

    connect(timer, SIGNAL(timeout()), this, SLOT(processMessages()));
    connect(timer2, SIGNAL(timeout()), this, SLOT(timer2timeout()));
    ui->AudioTree->setCurrentItem(ui->AudioTree->currentItem());
}

MainWindow::~MainWindow() {
    delete ui;
    timer->~QTimer();
    timer2->~QTimer();
}

void MainWindow::closeEvent(QCloseEvent *) {
    emit terminatingApp();
    QApplication::quit();
}

// envoyer message socket
// varmap ??
void MainWindow::playbuttonClicked(bool isPlaying) {
    QVariantMap varmap;
   // if (ui->AudioTree->selectedItems().size() != 0) {
    //    ui->AudioTree->setCurrentItem(ui->AudioTree->currentItem());
     //   varmap[kParamPath] = ui->AudioTree->currentItem()->data(0,Qt::UserRole);
     //   emit signalUI(kSignalChangeAudio,varmap);
    //} else {
        varmap[kParamPath] = ui->AudioTree->currentItem()->data(0,Qt::UserRole);
        varmap[kParamLength] = duree;
        if (isPlaying) {
            emit signalUI(kSignalPlay,varmap);
        } else {
            timer->stop();
            timer2->stop();
            emit signalUI(kSignalPause,varmap);
        }
   // }
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

void MainWindow::audioProgressClicked(int t) {
    QVariantMap varmap;
    QString dureeString;
    varmap[kParamTime] = t;
    evolutionTimer(duree-t); //changer temps restant...
    minutes = t/60;
    secondes = t%60;
    if (secondes<10)
        dureeString = QString::number(minutes)+":0"+QString::number(secondes);
    else
        dureeString = QString::number(minutes)+":"+QString::number(secondes);
    ui->tempsPasse->setText(dureeString);
    emit signalUI(kSignalTime,varmap);
}

// message du serveur
void MainWindow::message(signalType sig, QVariantMap params) {
    QString path;
    switch(sig) {
        case kSignalPlay:
            path = params[kParamPath].toString();
            ajouterAHistorique(path);
            break;
        case kSignalPause:
            timer->stop();
            timer2->stop();
            break;
        case kSignalEndPause:
            timer->start();
            timer2->start();
            break;
        case kSignalVolume:
        case kSignalMute:
        case kSignalUnmute:
            emit changeVolumeBar(params[kParamVolume].toInt());
            break;
        case kSignalGetProperties:
        qDebug() << "len" << params[kParamLength].toInt();
        qDebug() << "time" << params[kParamTime].toInt();
            path= params[kParamPath].toString();
            duree = params[kParamLength].toInt();
            if (params[kParamEtat].toInt() == kStatePlay) {
                emit changeButtonState(true);
                audioProgressClicked(params[kParamTime].toInt());
                timer->start();
            }
            emit changeVolumeBar(params[kParamVolume].toInt());
            emit changeMaxTimeBar(duree);
            emit changeTimeBar(params[kParamTime].toInt());
            break;
        case kSignalTime:
            qDebug() << "got time signal, time: " << params[kParamTime].toInt() ;
            emit changeTimeBar(params[kParamTime].toInt());
        default:
            break;
    }
}

void MainWindow::audioDoubleClicked(QTreeWidgetItem *item, int column) {
    QVariantMap varmap;
    QString dureeString;
    bool ok;
    varmap[kParamPath] = item->data(column,Qt::UserRole);
    duree = (item->text(1)).toInt(&ok,10);
    varmap[kParamLength] = duree;
    int s=0, m=0; //pour le temps restant
    m = duree/60;
    s = duree%60;
    if (s<10)
        dureeString = QString::number(m)+":0"+QString::number(s);
    else
        dureeString = QString::number(m)+":"+QString::number(s);
    minutes = 0;
    secondes = 0;
    ui->tempsPasse->setText("0:00");
    ui->tempsRestant->setText(dureeString);
    evolutionTimer(duree);
    emit changeMaxTimeBar(duree);
    emit changeButtonState(true);
    emit changeTimeBar(0);
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

void MainWindow::evolutionTimer(int duration){
    timer->stop();
    timer2->stop();
    timer->setInterval(1000);
    timer2->setInterval(duration*1000);
    timer->start(1000);
    timer2->start(duration*1000);
}

void MainWindow::processMessages(){

    QString temps_restant = ""+QString::number((timer2->remainingTime()/1000)/60)+":"+QString::number((timer2->remainingTime()/1000)%60);
    secondes++;
    if(secondes==60){
        minutes++;
        secondes=0;
    }

    QString temps_passe =""+QString::number(minutes)+":"+QString::number(secondes);
    if(secondes<10)
        temps_passe =""+QString::number(minutes)+":0"+QString::number(secondes);
    ui->tempsRestant->setText(temps_restant);
    ui->tempsPasse->setText(temps_passe);
    emit addSecond();
}

void MainWindow::timer2timeout() {
    qDebug() << "timer 2";
    duree=0;
    timer->stop();
    timer2->stop();
}

void MainWindow::on_actionAnglais_triggered()
{
    QTranslator translator;
    translator.load("../Client/client_en.qm"); // A voir  sur MAC
    QApplication::installTranslator(&translator);
    //QApplication::removeTranslator(&translator);
    ui->retranslateUi(this);
}

void MainWindow::on_actionFran_ais_triggered()
{
    QTranslator translator;
    translator.load("../Client/client_en.qm"); // A voir  sur MAC
    //QApplication::installTranslator(&translator);
    QApplication::removeTranslator(&translator);
    ui->retranslateUi(this);
    this->setWindowTitle(tr("Audio Player"));
}
