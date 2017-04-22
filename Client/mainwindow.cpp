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
#include <stdlib.h>
#include <QFileInfo>

using namespace TagLib;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Audio Player");

    int i;
    for (i=0; audio_files[i] != nullptr; i++) {
        QString filepath = "../../../../";
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
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *) {
    emit terminatingApp();
}

// envoyer message socket
// varmap ??
void MainWindow::playbuttonClicked(bool isPlaying) {
    QVariantMap varmap;
    varmap[kParamPath] = ui->AudioTree->currentItem()->data(0,Qt::UserRole);

    if (isPlaying) {
        emit signalUI(kSignalPlay,varmap);
    } else {
        emit signalUI(kSignalPause,varmap);
    }
}

// message du serveur
void MainWindow::message(signalType, QVariantMap) {
}

void MainWindow::audioDoubleClicked(QTreeWidgetItem *item, int column) {
qDebug() << "double click on audio";
    QVariantMap varmap;
    varmap[kParamPath] = item->data(column,Qt::UserRole);
    emit changeButtonState(true);
    emit signalUI(kSignalChangeAudio,varmap);
}
