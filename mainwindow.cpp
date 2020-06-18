#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLayout>
#include <QStandardPaths>
#include <QGraphicsVideoItem>

#include <QAbstractItemView>
#include <QModelIndex>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel();
    playlist = new QMediaPlaylist();
    player = new QMediaPlayer(this);
    player->setPlaylist(playlist);
    player->setVideoOutput(ui->videoWidget);

    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::openButtonClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::playButtonClicked);
    connect(ui->muteButton, &QPushButton::clicked, this, &MainWindow::muteButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &MainWindow::removeButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearButtonClicked);
    connect(ui->soundSlider, &QAbstractSlider::valueChanged, this, &MainWindow::soundSliderValueChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(ui->slider, &QAbstractSlider::sliderMoved, this, &MainWindow::sliderMoved);
    connect(ui->listView, &QListView::doubleClicked, this, &MainWindow::listViewClicked);

    db = QSqlDatabase::addDatabase("QSQLITE");
    QString path = QCoreApplication::applicationDirPath() + "/db.db";
    db.setDatabaseName(path);
    QSqlQuery query;
    if (!QFile::exists(path))
    {
        db.open();
        query.exec("CREATE TABLE 'Playlist' ('Path' TEXT, 'Filename' TEXT, 'Seconds' INTEGER)");
        db.close();
    }
    else
    {
        db.open();
        query.exec("SELECT * FROM 'Playlist'");
        for(int i = 0; query.next(); i++)
        {
            PlayListElement element;
            element.path = QUrl(query.value(0).toString());
            element.filename = query.value(1).toString();
            element.seconds = query.value(2).toInt();
            playListElements.push_back(element);

            playlist->addMedia(element.path);
            QStandardItem *item;
            item = new QStandardItem(element.filename);
            item->setEditable(false);
            model->appendRow(item);
        }
        db.close();
        ui->listView->setModel(model);
    }

    for(int i = 0; i < playListElements.size(); i++)
        qDebug() << playListElements[i].seconds;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    /**
    * @brief Обработчик закрытия формы
    * @param event Указатель на событие закрытия
    */
    if(player->state() != QMediaPlayer::StoppedState)
        playListElements[playlist->currentIndex()].seconds = player->position();
    db.open();
    QSqlQuery query;
    query.exec("DELETE FROM 'Playlist'");
    for(int i = 0; i < playListElements.size(); i++)
        query.exec("INSERT INTO 'Playlist' VALUES ('" + playListElements[i].path.toString() + "', '" + playListElements[i].filename + "', " + QString::number(playListElements[i].seconds) + ");");
    db.close();
}

void MainWindow::removeButtonClicked()
{
    /**
    * @brief Обработчик нажатия кнопки удаления элемента из плейлиста
    */
    if(!playlist->isEmpty() && ui->listView->currentIndex().row() != -1 && playListElements.size() != 1)
    {
        if(ui->listView->currentIndex().row() == playlist->currentIndex())
        {
            player->stop();
            ui->playButton->setText(">");
        }
        playListElements.remove(ui->listView->currentIndex().row());
        playlist->removeMedia(ui->listView->currentIndex().row());
        model->removeRow(ui->listView->currentIndex().row());
    }
}

void MainWindow::clearButtonClicked()
{
    /**
    * @brief Обработчик нажатия кнопки закрытия плейлиста
    */
    if(playListElements.size() != 1)
    {
        player->stop();
        ui->playButton->setText(">");
        playlist->clear();
        playListElements.remove(0, playListElements.size());
        model->clear();
        ui->listView->setModel(model);
    }
}

void MainWindow::openButtonClicked()
{
    /**
    * @brief Обработчик нажатия кнопки открытия файла
    */
    QFileDialog fileDialog;
    QUrl path = fileDialog.getOpenFileUrl();

    PlayListElement element;
    element.path = path;
    element.filename = QString(path.url().remove(0, path.url().lastIndexOf('/')+1));
    element.seconds = 0;
    playListElements.push_back(element);

    playlist->addMedia(QUrl(path));
    QStandardItem *item;
    item = new QStandardItem(element.filename);
    item->setEditable(false);
    model->appendRow(item);
    ui->listView->setModel(model);
}

void MainWindow::listViewClicked(QModelIndex modelIndex)
{
    /**
    * @brief Обновление нажатия на элемент плейлиста
    */
    int secs = playListElements[modelIndex.row()].seconds;
    if(player->state() != QMediaPlayer::StoppedState)
        playListElements[playlist->currentIndex()].seconds = player->position();
    playlist->setCurrentIndex(modelIndex.row());
    startPlayer();
    player->setPosition(secs);
}

void MainWindow::startPlayer()
{
    /**
    * @brief Запуск плеера
    */
    if(!playlist->isEmpty())
    {
        player->play();
        ui->playButton->setText("||");
    }
}

void MainWindow::stopPlayer()
{
    /**
    * @brief Остановка плеера
    */
    player->pause();
    ui->playButton->setText(">");
}

void MainWindow::playButtonClicked()
{
    /**
    * @brief Обработчик нажатия кнопки запуска плеера
    */
    if(player->state() == QMediaPlayer::PlayingState)
        stopPlayer();
    else
        startPlayer();
}

void MainWindow::muteButtonClicked()
{
    /**
    * @brief Обработчик нажатия кнопки выключения звука
    */
    if(player->isMuted())
    {
        player->setMuted(true);
        ui->muteButton->setText("Off");
    }
    else
    {
        player->setMuted(true);
        ui->muteButton->setText("On");
    }
}

void MainWindow::soundSliderValueChanged()
{
    /**
    * @brief Обработчик события изменения громкости
    */
    player->setVolume(ui->soundSlider->value());
}

void MainWindow::positionChanged()
{
    /**
    * @brief Обработчик события изменения текущей позиции видео
    */
    ui->slider->setSliderPosition((double)player->position()/(double)(player->duration()+1)*1000);
    ui->slider->update();
    playListElements[playlist->currentIndex()].seconds = player->position();
    isAutomaticalValueChange = true;
}

void MainWindow::sliderMoved()
{
    /**
    * @brief Обработчик события изменения текущей позиции видео пользователем
    */
    player->setPosition(player->duration()*(float)(ui->slider->value()+1)/1000);
}
