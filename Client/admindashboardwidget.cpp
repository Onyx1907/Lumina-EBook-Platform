#include "admindashboardwidget.h"
#include "ui_admindashboardwidget.h"
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

AdminDashboardWidget::AdminDashboardWidget(Admin *admin, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminDashboardWidget)
{
    ui->setupUi(this);

    AdminBooksPage = new AdminBooksWidget(this);
    EditBookPage = new EditBookWidget(this);

    ui->stackedWidget->addWidget(AdminBooksPage);
    ui->stackedWidget->addWidget(EditBookPage);

    ui->stackedWidget->setCurrentIndex(Page::AdminBooksPageIndex);

    //رفتن به صفحه ویرایش کتاب
    connect(AdminBooksPage, &AdminBooksWidget::editBook, this, [this](int bookID){
        EditBookPage->loadEditBook(bookID);
        fadeToPage(Page::EditBookPageIndex);
    });

    //برگشت از صفحه ویرایش کتاب
    connect(EditBookPage, &EditBookWidget::back, this, [this](){
        fadeToPage(Page::AdminBooksPageIndex);
    });
}

AdminDashboardWidget::~AdminDashboardWidget()
{
    delete ui;
}


void AdminDashboardWidget::fadeToPage(int pageIndex) {
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

void AdminDashboardWidget::on_books_pushButton_clicked()
{
    fadeToPage(Page::AdminBooksPageIndex);
}

