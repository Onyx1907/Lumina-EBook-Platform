#include "userdashboardwidget.h"
#include "ui_userdashboardwidget.h"
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

UserDashboardWidget::UserDashboardWidget(RegularUser* cur_user, bool is_first_login, QWidget *parent)
    : QWidget(parent) , user(cur_user)
    , ui(new Ui::UserDashboardWidget)
{
    ui->setupUi(this);

    //setAttribute(Qt::WA_TranslucentBackground);
    if (this->layout()) {
        delete this->layout();
    }

    ui->containerWidget->setParent(this);
    ui->containerWidget->setFixedSize(900, 650); // یا هر سایزی که در دیزاینر مد نظرت هست
    QGridLayout* finalLayout = new QGridLayout(this);
    finalLayout->addWidget(ui->containerWidget, 0, 0, Qt::AlignCenter);
    this->setLayout(finalLayout);

    GenreSelectionPage = new GenreSelectionWidget(user->getUsername(), this);
    UserHomePage = new UserHomeWidget(user->getId(), this);
    ProfilePage = new ProfileWidget(user, this);
    BookDetailsPage = new BookDetailsWidget(user->getId(), this);

    ui->stackedWidget->addWidget(GenreSelectionPage);
    ui->stackedWidget->addWidget(UserHomePage);
    ui->stackedWidget->addWidget(ProfilePage);
    ui->stackedWidget->addWidget(BookDetailsPage);

    if(is_first_login){
        ui->stackedWidget->setCurrentIndex(Page::GenreSelectionPageIndex);
        for(QAbstractButton *button : ui->sidebar_buttonGroup->buttons()){
            button->setEnabled(false);
        }
    }
    else{
        ui->stackedWidget->setCurrentIndex(Page::UserHomePageIndex);
        for(QAbstractButton *button : ui->sidebar_buttonGroup->buttons()){
            button->setEnabled(true);
        }
        ui->home_pushButton->setChecked(true);
    }

    connect(GenreSelectionPage, &GenreSelectionWidget::goToHomePage, this, [this](){
        fadeToPage(Page::UserHomePageIndex);
        for(QAbstractButton *button : ui->sidebar_buttonGroup->buttons()){
            button->setEnabled(true);
        }
        ui->home_pushButton->setChecked(true);
    });

    connect(ProfilePage, &ProfileWidget::goToGenreSelectionPage, this, [this](){
        fadeToPage(Page::GenreSelectionPageIndex);
        GenreSelectionPage->onGeresChangeClicked();
    });

    connect(UserHomePage, &UserHomeWidget::bookSelected, this, [this]( Book* book){
        previousPageIndex = UserHomePageIndex;
        BookDetailsPage->loadBook(book);
        fadeToPage(BookDetailsPageIndex);
    });

    connect(BookDetailsPage, &BookDetailsWidget::backPrevious, this, [this](){
        fadeToPage(previousPageIndex);
    });
}



void UserDashboardWidget::fadeToPage(int pageIndex) {
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


UserDashboardWidget::~UserDashboardWidget()
{
    delete ui;
}

void UserDashboardWidget::on_home_pushButton_clicked()
{
    fadeToPage(Page::UserHomePageIndex);
}


void UserDashboardWidget::on_profile_pushButton_clicked()
{
    ProfilePage->loadProfile();
    fadeToPage(Page::ProfilePageIndex);
}

