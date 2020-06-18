#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <PlayListElement.h>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QFileDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QStandardItemModel>
#include <QStandardItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QStandardItemModel *model;
    bool isAutomaticalValueChange = false;
    QVector<PlayListElement> playListElements;
    QSqlDatabase db;

    void closeEvent(QCloseEvent *event);
    void startPlayer();
    void stopPlayer();

private Q_SLOTS:
    void removeButtonClicked();
    void clearButtonClicked();
    void openButtonClicked();
    void listViewClicked(QModelIndex modelIndex);
    void playButtonClicked();
    void muteButtonClicked();
    void soundSliderValueChanged();
    void positionChanged();
    void sliderMoved();
};
#endif // MAINWINDOW_H
