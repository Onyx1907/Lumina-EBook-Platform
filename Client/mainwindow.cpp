#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QPainter>
#include "clientnetworkmanager.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setMinimumSize(900, 650);
    this->resize(900, 650);

    // ۱. ساخت پلیر و سینک گرافیکی
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0);
    mediaPlayer->setAudioOutput(audioOutput);

    videoSink = new QVideoSink(this);
    mediaPlayer->setVideoOutput(videoSink);

    // 🔹 ۲. شکار کردن فریم‌های ویدیو به محض آماده شدن
    connect(videoSink, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame) {
        if (frame.isValid()) {
            // تبدیل فریم ویدیو به عکس قابل نقاشی در کیوت
            QImage img = frame.toImage();
            currentFramePixmap = QPixmap::fromImage(img);
            this->update(); // به مِین‌ویندو دستور بده خودش را دوباره نقاشی کند
        }
    });

    mediaPlayer->setSource(QUrl("qrc:/resources/background.webm"));
    mediaPlayer->setLoops(QMediaPlayer::Infinite);
    mediaPlayer->play();

    // ۳. شفاف‌سازی استک‌ویجت
    ui->stackedWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->stackedWidget->setStyleSheet("background: transparent; QWidget { background: transparent; }");

    LoginPage = new LoginWidget(this);
    RegisterPage = new RegisterWidget(this);
    ForgotPasswordPage = new ForgotPasswordWidget(this);

    ui->stackedWidget->addWidget(LoginPage);
    ui->stackedWidget->addWidget(RegisterPage);
    ui->stackedWidget->addWidget(ForgotPasswordPage);

    ui->stackedWidget->setCurrentIndex(Page::LoginPageIndex);
    ui->stackedWidget->raise();


    connect(LoginPage, &LoginWidget::goToRegisterRequested, this, [this](){
        ui->stackedWidget->setCurrentIndex(Page::RegisterPageIndex);
    });

    connect(LoginPage, &LoginWidget::goToForgotPasswordRequested, this, [this](){
        ui->stackedWidget->setCurrentIndex(Page::ForgotPasswordPageIndex);
    });

}

// 🔹 ۵. جادوی اصلی: نقاشی کردن فریم ویدیو در دورترین لایه عقب مِین‌ویندو
void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    if (!currentFramePixmap.isNull()) {
        // نقاشی کردن عکس ویدیو به اندازه کل پنجره
        painter.drawPixmap(0, 0, this->width(), this->height(), currentFramePixmap);
    } else {
        // اگر هنوز فریم لود نشده، یک رنگ پس‌زمینه موقت شکلاتی/تیره بینداز
        painter.fillRect(this->rect(), QColor(44, 34, 30));
    }
}

// ۶. تابع ریسایز خیلی سبک‌تر می‌شود
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    this->update(); // صرفاً درخواست بازنویسی صفحه
}


MainWindow::~MainWindow()
{
    delete ui;
}
