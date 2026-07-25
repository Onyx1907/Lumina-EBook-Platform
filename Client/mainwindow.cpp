#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include "clientnetworkmanager.h"
#include "publisher.h"


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

    // شفاف‌سازی استک‌ویجت
    ui->stackedWidget->setStyleSheet("background: transparent; QWidget { background: transparent; }");

    LoginPage = new LoginWidget(this);
    RegisterPage = new RegisterWidget(this);
    ForgotPasswordPage = new ForgotPasswordWidget(this);

    ui->stackedWidget->addWidget(LoginPage);
    ui->stackedWidget->addWidget(RegisterPage);
    ui->stackedWidget->addWidget(ForgotPasswordPage);

    fadeToPage(Page::LoginPageIndex);
    ui->stackedWidget->raise();


    connect(LoginPage, &LoginWidget::goToRegisterRequested, this, [this](){
        fadeToPage(Page::RegisterPageIndex);
    });

    connect(LoginPage, &LoginWidget::goToForgotPasswordRequested, this, [this](){
        fadeToPage(Page::ForgotPasswordPageIndex);
    });

    connect(RegisterPage, &RegisterWidget::goToLoginRequested, this, [this](){
        fadeToPage(Page::LoginPageIndex);
    });

    connect(ForgotPasswordPage, &ForgotPasswordWidget::goToLoginRequested, this, [this](){
        fadeToPage(Page::LoginPageIndex);
    });

    connect(LoginPage, &LoginWidget::goToUSerDashboard, this, [this](User *user, bool is_first_login){

        mediaPlayer->stop();
        mediaPlayer->setSource(QUrl("qrc:/resources/dashboard.mp4"));
        mediaPlayer->play();

        RegularUser *cur_user = dynamic_cast<RegularUser*>(user);
        Publisher *cur_user2 = dynamic_cast<Publisher*>(user);

        if(cur_user != nullptr) {
            UserDashboardPage = new UserDashboardWidget(cur_user, is_first_login, this);

            ui->stackedWidget->addWidget(UserDashboardPage);

            fadeToPage(Page::UserDashboardPageIndex);
        }
        else if(cur_user2 != nullptr){
            PublisherDashboardPage = new PublisherDashboardWidget(cur_user2, this);

            ui->stackedWidget->addWidget(PublisherDashboardPage);

            fadeToPage(Page::UserDashboardPageIndex);
        }

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

void MainWindow::fadeToPage(int pageIndex) {
    QWidget *currentWidget = ui->stackedWidget->currentWidget();
    QWidget *nextWidget = ui->stackedWidget->widget(pageIndex);

    if (!currentWidget || !nextWidget || currentWidget == nextWidget) return;

    // ۱. ایجاد افکت شفافیت برای هر دو صفحه
    QGraphicsOpacityEffect *fadeOutEffect = new QGraphicsOpacityEffect(currentWidget);
    QGraphicsOpacityEffect *fadeInEffect = new QGraphicsOpacityEffect(nextWidget);

    currentWidget->setGraphicsEffect(fadeOutEffect);
    nextWidget->setGraphicsEffect(fadeInEffect);

    // ۲. انیمیشن محو شدن صفحه فعلی (مدت زمان: ۳۰۰ میلی‌ثانیه)
    QPropertyAnimation *fadeOutAnim = new QPropertyAnimation(fadeOutEffect, "opacity");
    fadeOutAnim->setDuration(300);
    fadeOutAnim->setStartValue(1.0);
    fadeOutAnim->setEndValue(0.0);

    // ۳. انیمیشن ظاهر شدن صفحه بعدی (مدت زمان: ۳۰۰ میلی‌ثانیه)
    QPropertyAnimation *fadeInAnim = new QPropertyAnimation(fadeInEffect, "opacity");
    fadeInAnim->setDuration(300);
    fadeInAnim->setStartValue(0.0);
    fadeInAnim->setEndValue(1.0);

    // ۴. گروه‌بندی انیمیشن‌ها به صورت متوالی (اول اولی محو شه، بعد دومی بیاد)
    QSequentialAnimationGroup *group = new QSequentialAnimationGroup(this);
    group->addAnimation(fadeOutAnim);

    // دقیقاً وسط کار (وقتی صفحه اول محو شد)، ایندکس استک‌ویجت رو عوض می‌کنیم
    connect(fadeOutAnim, &QPropertyAnimation::finished, this, [this, pageIndex]() {
        ui->stackedWidget->setCurrentIndex(pageIndex);
    });

    group->addAnimation(fadeInAnim);

    // ۵. پاک‌سازی افکت‌ها بعد از پایان کل انیمیشن برای جلوگیری از کندی گرافیکی
    connect(group, &QSequentialAnimationGroup::finished, this, [currentWidget, nextWidget, group]() {
        currentWidget->setGraphicsEffect(nullptr);
        nextWidget->setGraphicsEffect(nullptr);
        group->deleteLater(); // حذف خودکار گروه انیمیشن از رم
    });

    // شروع انیمیشن
    group->start();
}


MainWindow::~MainWindow()
{
    delete ui;
}
