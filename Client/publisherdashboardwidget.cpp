#include "publisherdashboardwidget.h"
#include "ui_publisherdashboardwidget.h"
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QQuickWidget>
#include <QQuickItem>

PublisherDashboardWidget::PublisherDashboardWidget(Publisher *cur_user, QWidget *parent)
    : QWidget(parent), publisher(cur_user)
    , ui(new Ui::PublisherDashboardWidget)
{
    ui->setupUi(this);

    if (this->layout()) {
        delete this->layout();
    }

    ui->containerWidget->setParent(this);
    ui->containerWidget->setFixedSize(900, 650); // یا هر سایزی که در دیزاینر مد نظرت هست
    QGridLayout* finalLayout = new QGridLayout(this);
    finalLayout->addWidget(ui->containerWidget, 0, 0, Qt::AlignCenter);
    this->setLayout(finalLayout);

    PublisherStatsPage = new PublisherStatsWidget(publisher->getId(), this);
    ProfilePage = new ProfileWidget(publisher, this);
    PublisherBookPage = new PublisherBookWidget(publisher->getId(), this);
    EditBookPage = new EditBookWidget(this, publisher->getId());
    BookReaderPage = new BookReaderWidget(-1, this);
    CommentsPage = new CommentsWidget(publisher->getId(), this, true);
    NotificationsPage = new NotificationsWidget(publisher->getId(), "Publisher", this);

    ui->stackedWidget->addWidget(PublisherStatsPage);
    ui->stackedWidget->addWidget(ProfilePage);
    ui->stackedWidget->addWidget(PublisherBookPage);
    ui->stackedWidget->addWidget(EditBookPage);
    ui->stackedWidget->addWidget(BookReaderPage);
    ui->stackedWidget->addWidget(CommentsPage);
    ui->stackedWidget->addWidget(NotificationsPage);

    QQuickWidget *disconnectedPage = new QQuickWidget(this);

    disconnectedPage->setResizeMode(QQuickWidget::SizeRootObjectToView);
    disconnectedPage->setSource(QUrl("qrc:/qml/Disconnected.qml"));

    ui->stackedWidget->addWidget(disconnectedPage);

    ui->notif_widget->setID(publisher->getId());

    ui->notif_widget->sendUnreadNotifsRequest();

    ui->home_pushButton->setChecked(true);
    ui->stackedWidget->setCurrentIndex(Page::PublisherStatsPageIndex);

    //ایجاد کتاب جدید
    connect(PublisherBookPage, &PublisherBookWidget::addBook, this, [this](){
        EditBookPage->loadEditBook();
        fadeToPage(EditBookPageIndex);
    });

    connect(EditBookPage , &EditBookWidget::back, this, [this](){
       fadeToPage(PublisherBookPageIndex);
    });

    //ویرایش کتاب
    connect(PublisherBookPage, &PublisherBookWidget::editBook, this, [this](int bookID){
        EditBookPage->loadEditBook(bookID);
        fadeToPage(EditBookPageIndex);
    });

    //رفتن به پی دی اف کتاب
    connect(PublisherBookPage, &PublisherBookWidget::PDFreader, this, [this](QString path){
        BookReaderPage->loadBook(path);
        fadeToPage(BookReaderPageIndex);
    });

    connect(BookReaderPage, &BookReaderWidget::back, this, [this](){
       fadeToPage(PublisherBookPageIndex);
    });

    //رفتن به همه کتاب ها از طریق صفحه داشبورد
    connect(PublisherStatsPage, &PublisherStatsWidget::goToBooks, this, [this](){
       fadeToPage(PublisherBookPageIndex);
    });

    //رفتن به صفحه نظرات
    connect(PublisherBookPage, &PublisherBookWidget::goToCommentsPage,this, [this](int bookID){
        CommentsPage->loadComments(bookID);
        fadeToPage(Page::CommentsPageIndex);
    });

    connect(CommentsPage, &CommentsWidget::backToBookDatailPage, this, [this](){
        fadeToPage(PublisherBookPageIndex);
    });

    //رفتن به صفحه اعلان ها
    connect(ui->notif_widget, &NotificationBar::displayNotifs, this, [this](){
        NotificationsPage->loadNotifs();
        ui->notif_widget->sendUnreadNotifsRequest();
        if(ui->stackedWidget->currentIndex() != Page::NotificationsPageIndex){
            prevNotifs = ui->stackedWidget->currentIndex();
        }
        fadeToPage(Page::NotificationsPageIndex);
    });

    //برگشت از صفحه اعلان ها
    connect(NotificationsPage, &NotificationsWidget::back, this, [this](){
        ui->notif_widget->sendUnreadNotifsRequest();
        fadeToPage(prevNotifs);
    });

    //کم شدن عدد اعلان ها
    connect(NotificationsPage, &NotificationsWidget::decreaseCount, this, [this](){
        ui->notif_widget->decreaseUnreadCount();
    });

    //نمایش صفحه قطع اتصال
    connect(PublisherStatsPage, &PublisherStatsWidget::disconnected, this, [this](){
        ui->stackedWidget->setCurrentIndex(Page::disconnectedPageIndex);
        for(QAbstractButton *button : ui->sidebar_buttonGroup->buttons()){
            button->setEnabled(false);
        }

        ui->notif_widget->setEnableButton(false);
    });

    //تلاش مجدد
    if (disconnectedPage->rootObject()) {
        connect(disconnectedPage->rootObject(), SIGNAL(retryClicked()), this, SLOT(onRetryClicked()));
    }
}

PublisherDashboardWidget::~PublisherDashboardWidget()
{
    delete ui;
}


void PublisherDashboardWidget::fadeToPage(int pageIndex) {
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
    fadeOutAnim->setDuration(200);
    fadeOutAnim->setStartValue(1.0);
    fadeOutAnim->setEndValue(0.0);

    // ۳. انیمیشن ظاهر شدن صفحه بعدی (مدت زمان: ۳۰۰ میلی‌ثانیه)
    QPropertyAnimation *fadeInAnim = new QPropertyAnimation(fadeInEffect, "opacity");
    fadeInAnim->setDuration(200);
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

void PublisherDashboardWidget::on_profile_pushButton_clicked()
{
    ProfilePage->loadProfile();
    fadeToPage(Page::ProfilePageIndex);
}


void PublisherDashboardWidget::on_books_pushButton_clicked()
{
    fadeToPage(Page::PublisherBookPageIndex);
}


void PublisherDashboardWidget::on_home_pushButton_clicked()
{
    fadeToPage(Page::PublisherStatsPageIndex);
}


void PublisherDashboardWidget::onRetryClicked(){

    for(QAbstractButton *button : ui->sidebar_buttonGroup->buttons()){
        button->setEnabled(true);
    }
    ui->notif_widget->setEnableButton(true);

    QTimer::singleShot(0, this, [this](){
        ui->stackedWidget->setCurrentIndex(Page::PublisherStatsPageIndex);
    });
}
