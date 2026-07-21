#include "publisherdashboardwidget.h"
#include "ui_publisherdashboardwidget.h"
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

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

    ProfilePage = new ProfileWidget(publisher, this);
    PublisherBookPage = new PublisherBookWidget(publisher->getId(), this);
    EditBookPage = new EditBookWidget(this, publisher->getId());
    BookReaderPage = new BookReaderWidget(-1, this);

    ui->stackedWidget->addWidget(ProfilePage);
    ui->stackedWidget->addWidget(PublisherBookPage);
    ui->stackedWidget->addWidget(EditBookPage);
    ui->stackedWidget->addWidget(BookReaderPage);

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

