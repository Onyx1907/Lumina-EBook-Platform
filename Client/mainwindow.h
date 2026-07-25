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
#include "userdashboardwidget.h"
#include "publisherdashboardwidget.h"
#include "admindashboardwidget.h"


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

private slots:

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer = nullptr;
    QAudioOutput *audioOutput = nullptr;
    QVideoSink *videoSink = nullptr;
    QPixmap currentFramePixmap;

    LoginWidget *LoginPage;
    RegisterWidget *RegisterPage;
    ForgotPasswordWidget *ForgotPasswordPage;
    UserDashboardWidget *UserDashboardPage = nullptr;
    PublisherDashboardWidget *PublisherDashboardPage = nullptr;
    AdminDashboardWidget *AdminDashboardPage = nullptr;

    void fadeToPage(int pageIndex);

    enum Page {
        LoginPageIndex = 0,
        RegisterPageIndex = 1,
        ForgotPasswordPageIndex = 2,
        UserDashboardPageIndex = 3
    };
};

#endif // MAINWINDOW_H
