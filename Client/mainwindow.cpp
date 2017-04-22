/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Audio Player");

    int i;
    for (i=0; audio_files[i] != nullptr; i++) {
        QTreeWidgetItem *audio = new QTreeWidgetItem(ui->AudioTree);
        audio->setText(0, tr(audio_files[i]));
    }

    for (i=0; radio_links[i] != nullptr; i++) {
        ui->RadioList->addItem(radio_links[i]);
    }

    connect(ui->playbutton,SIGNAL(changedState(bool)),this,SLOT(playbuttonClicked(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// envoyer message socket
// varmap ??
void MainWindow::playbuttonClicked(bool isPlaying) {
    QVariantMap varmap;
    varmap[kParamPath] = QVariant(ui->AudioTree->currentItem()->text(0));

    if (isPlaying) {
        emit signalUI(kSignalPlay,varmap);
    } else {
        emit signalUI(kSignalPause,varmap);
    }
}

// message du serveur
void MainWindow::message(signalType, QVariantMap) {
}
