#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QVideoSink>
#include <QVideoFrame>
#include "loginwidget.h"
#include "registerwidget.h"
#include "forgotpasswordwidget.h"
#include "page_indices.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



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

    LoginWidget *LoginPage;
    RegisterWidget *RegisterPage;
    ForgotPasswordWidget *ForgotPasswordPage;
};

#endif // MAINWINDOW_H
