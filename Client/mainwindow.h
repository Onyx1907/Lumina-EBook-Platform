#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QVideoSink>
#include <QVideoFrame>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    enum Page {
        LoginPageIndex = 0,
        RegisterPageIndex = 1,
        ForgotPasswordPageIndex = 2
    };

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;
    QWidget *bgWidget = nullptr;
    QVideoSink *videoSink = nullptr;
    QPixmap currentFramePixmap;
};

#endif // MAINWINDOW_H
