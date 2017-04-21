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
    if (isPlaying)
        emit signalUI(kSignalPlay,varmap);
    else
        emit signalUI(kSignalPause,varmap);
}

// message du serveur
void MainWindow::message(signalType, QVariantMap) {
}
