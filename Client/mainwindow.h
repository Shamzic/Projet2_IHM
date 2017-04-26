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

signals:
    // Le format de communication vers le client
    void signalUI(signalType, QVariantMap);
    void terminatingApp();
    void changeButtonState(bool);
    void changeVolumeBar(int);

public slots:
    // Messages reçus du client
    void message(signalType, QVariantMap);
    void playbuttonClicked(bool);
    void volumeBarClicked(int);
    void closeEvent(QCloseEvent *event);

private slots:
    void audioDoubleClicked(QTreeWidgetItem *item, int column);
    void on_muteButton_clicked();
};

#endif // MAINWINDOW_H
