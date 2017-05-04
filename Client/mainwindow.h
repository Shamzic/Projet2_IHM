/***************************************************************************
 *                                                                         *
 *   Gomes Fernandes Caty, Hamery Simon                                    *
 *   L3 Informatique, S6 Printemps                                         *
 *                                                                         *
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariantMap>
#include <QCloseEvent>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QTimer>

#include "client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool mute;
    QPixmap muteSymbol;
    QPixmap volumeOnSymbol;
    void ajouterAHistorique(QString path);
    QTimer *timer;
    QTimer *timer2;
    float secondes;
    float minutes;

signals:
    // Le format de communication vers le client
    void signalUI(signalType, QVariantMap);
    void terminatingApp();
    void changeButtonState(bool);
    void changeVolumeBar(int);
    void changeTimeBar(int);
    void changeMaxTimeBar(int);

public slots:
    // Messages reçus du client
    void message(signalType, QVariantMap);
    void playbuttonClicked(bool);
    void volumeBarClicked(int);
    void audioProgressClicked(int t);
    void closeEvent(QCloseEvent *event);
    void evolutionTimer(int start,int end);

private slots:
    void audioDoubleClicked(QTreeWidgetItem *item, int column);
    void on_muteButton_clicked();
    void processMessages();
};

#endif // MAINWINDOW_H
