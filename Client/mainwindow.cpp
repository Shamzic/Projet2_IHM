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
    duree(0),
    nom_morceau(""),
    ouverture(true)

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
            ui->AudioTree->currentItem()->setSelected(true);
            //qDebug()<<"audio à l'ini : "<<ui->AudioTree->currentItem()->text(0);

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
    ui->AudioTree->setCurrentItem(ui->AudioTree->topLevelItem(0));
    qDebug()<<"audio à l'ini : "<<ui->AudioTree->currentItem()->text(0);
    nom_morceau=ui->AudioTree->currentItem()->text(0);
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
        varmap[kParamLength] = ui->AudioTree->currentItem()->text(1).toInt();
        if (isPlaying) {
            timer2->setInterval(varmap[kParamLength].toInt()*1000);
            emit signalUI(kSignalPlay,varmap);
            qDebug()<<"current song selected : "<<ui->AudioTree->currentItem()->text(0);
            qDebug()<<"current nom morceau : "<<nom_morceau;
            if(ui->AudioTree->currentItem()->text(0)!=nom_morceau || (ouverture==true) )
            {
                ouverture=false;
                int col = 0;
               audioDoubleClicked(ui->AudioTree->currentItem(),col);
               qDebug()<<"playyy";
            }
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
    qDebug() << "duree :" << duree;
    qDebug() << "t :" << t;
    qDebug() << "tmps restant :" << duree-t;
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
        qDebug() << "end pause";
            timer->start();
            timer2->start();
            break;
        case kSignalVolume:
        case kSignalMute:
        case kSignalUnmute:
            emit changeVolumeBar(params[kParamVolume].toInt());
            break;
        case kSignalGetProperties:
            startSynchronize(params);
            break;
        case kSignalTime:
            qDebug() << "got time signal, time: " << params[kParamTime].toInt() ;
            emit changeTimeBar(params[kParamTime].toInt());
            break;
        case kSignalEnd:
            on_action_triggered();
            break;
        default:
            break;
    }
}

void MainWindow::audioDoubleClicked(QTreeWidgetItem *item, int column) {
    qDebug()<<"entrée double clic";
    QVariantMap varmap;
    QString dureeString;
    bool ok;
    varmap[kParamPath] = item->data(column,Qt::UserRole);
    nom_morceau=item->text(0);
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
        //emit changeVolumeBar(0);
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
    int m_restant, s_restant;
    m_restant =((timer2->remainingTime()/1000)/60);
    s_restant =((timer2->remainingTime()/1000)%60);
    QString temps_restant;
    if(s_restant<10) {
        temps_restant =""+QString::number(m_restant)+":0"+QString::number(s_restant);
    } else {
        temps_restant =""+QString::number(m_restant)+":"+QString::number(s_restant);
    }
    secondes++;
    if(secondes==60){
        minutes++;
        secondes=0;
    }

    QString temps_passe;
    if(secondes<10) {
        temps_passe =""+QString::number(minutes)+":0"+QString::number(secondes);
    } else {
        temps_passe =""+QString::number(minutes)+":"+QString::number(secondes);
    }
    ui->tempsRestant->setText(temps_restant);
    ui->tempsPasse->setText(temps_passe);
    emit addSecond();
}

void MainWindow::timer2timeout() {
    qDebug() << "timer 2";
    ui->tempsRestant->setText("0:00");
    duree=0;
    timer->stop();
    timer2->stop();
}

void MainWindow::on_actionAnglais_triggered()
{
    QTranslator translator;
#ifdef Q_OS_MAC
        translator.load("../../../../Client/client_en.qm");
#else
        translator.load("../Client/client_en.qm");
#endif
    QApplication::installTranslator(&translator);
    ui->retranslateUi(this);
}

void MainWindow::on_actionFran_ais_triggered()
{
    QTranslator translator;
    translator.load("../Client/client_en.qm"); // A voir  sur MAC
    QApplication::removeTranslator(&translator);
    ui->retranslateUi(this);
    this->setWindowTitle(tr("MPV-PLAYER-FRANCAIS"));
}

void MainWindow::on_action_triggered() {
    emit terminatingApp();
    QApplication::quit();
}

void MainWindow::on_fastForwButton_clicked()
{
    int i=0;
    //ui->AudioTree->setCurrentItem(audio->treeWidget()->topLevelItem(0));
    while(ui->AudioTree->topLevelItem(i)->text(0) != ui->AudioTree->currentItem()->text(0))
    {
        i++;
        qDebug()<<"i : "<<i;
       // Do something with item ...
    }

    ui->AudioTree->topLevelItem(i)->setSelected(false);
    i++; // on prends le suivant
    if(i==ui->AudioTree->topLevelItemCount())
    {
        qDebug()<<"topLevelItemCount() : "<<ui->AudioTree->topLevelItemCount();
        i=0; // On revient au début
    }
    qDebug()<<"hop suivant : "<<ui->AudioTree->topLevelItem(i)->text(0);
    ui->AudioTree->topLevelItem(i)->setSelected(true);
    ui->AudioTree->setCurrentItem(ui->AudioTree->topLevelItem(i));
    audioDoubleClicked(ui->AudioTree->topLevelItem(i),0);
}

void MainWindow::on_fastBackButton_clicked()
{
    int i=ui->AudioTree->topLevelItemCount()-1;
    //ui->AudioTree->setCurrentItem(audio->treeWidget()->topLevelItem(0));
    while(ui->AudioTree->topLevelItem(i)->text(0) != ui->AudioTree->currentItem()->text(0))
    {
        i--;
        qDebug()<<"i : "<<i;
    }

    ui->AudioTree->topLevelItem(i)->setSelected(false);
    i--; // on prends le suivant
    if(i<0)
    {
        qDebug()<<"arrive au début : ";
        i=ui->AudioTree->topLevelItemCount()-1; // On revient au début
    }
    qDebug()<<"hop suivant : "<<ui->AudioTree->topLevelItem(i)->text(0);
    ui->AudioTree->topLevelItem(i)->setSelected(true);
    ui->AudioTree->setCurrentItem(ui->AudioTree->topLevelItem(i));

    audioDoubleClicked(ui->AudioTree->topLevelItem(i),0);
}

void MainWindow::startSynchronize(QVariantMap params) {
    QString path = params[kParamPath].toString();
    emit changeVolumeBar(params[kParamVolume].toInt());

    qDebug()  << params[kParamTime].toInt() ;
    switch (params[kParamEtat].toInt()) {
        case kStateAttente:
        qDebug() << "ici1";
            minutes=0; secondes=0;
            break;
        case kStatePlay:
        case kStateReprendre:
        qDebug() << "ici2";
            duree = params[kParamLength].toInt();
            emit changeMaxTimeBar(duree);
            emit changeButtonState(true);
            //evolutionTimer(duree);
            //emit changeTimeBar(params[kParamTime].toInt());
            break;
        case kStatePause:
        qDebug() << "ici3";
            emit changeMaxTimeBar(duree);
            //emit changeTimeBar(params[kParamTime].toInt());
            //timer2->setInterval(duree-params[kParamTime].toInt()*1000);
            break;
        default:
            break;
    }
}
