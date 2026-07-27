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
    SearchPage = new SearchWidget(this);
    ResultPage = new ResultWidget(this);
    CommentsPage = new CommentsWidget(user->getId(), this);
    CartPage = new CartWidget(user->getId(), this);
    LibraryPage =  new LibraryWidget(user->getId(), this);
    BookReaderPage =  new BookReaderWidget(user->getId(), this);
    PurchaseHistoryPage = new PurchaseHistoryWidget(user->getId(), this);
    NotificationsPage = new NotificationsWidget(user->getId(), "RegularUser", this);

    ui->stackedWidget->addWidget(GenreSelectionPage);
    ui->stackedWidget->addWidget(UserHomePage);
    ui->stackedWidget->addWidget(ProfilePage);
    ui->stackedWidget->addWidget(BookDetailsPage);
    ui->stackedWidget->addWidget(SearchPage);
    ui->stackedWidget->addWidget(ResultPage);
    ui->stackedWidget->addWidget(CommentsPage);
    ui->stackedWidget->addWidget(CartPage);
    ui->stackedWidget->addWidget(LibraryPage);
    ui->stackedWidget->addWidget(BookReaderPage);
    ui->stackedWidget->addWidget(PurchaseHistoryPage);
    ui->stackedWidget->addWidget(NotificationsPage);

    ui->notif_widget->setID(user->getId());

    ui->notif_widget->sendUnreadNotifsRequest();

    //مدیریت نمایش صفحه انتخاب ژانر
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

    //نمایش صفحه انتخاب ژانر پس از کلیک روی ژانر های مورد علاقه در پروفایل
    connect(ProfilePage, &ProfileWidget::goToGenreSelectionPage, this, [this](){
        fadeToPage(Page::GenreSelectionPageIndex);
        GenreSelectionPage->onGeresChangeClicked();
    });

    //نمایش اطلاعات کتاب در صورت کلیک روی کتاب های صفحه اصلی
    connect(UserHomePage, &UserHomeWidget::bookSelected, this, [this]( Book* book){
        previousPageIndex = UserHomePageIndex;
        BookDetailsPage->loadBook(book);
        fadeToPage(BookDetailsPageIndex);
    });

    connect(BookDetailsPage, &BookDetailsWidget::backPrevious, this, [this](){
        fadeToPage(previousPageIndex);
    });

    //نمایش صفحه نتایج پس از سرچ
    connect(SearchPage, &SearchWidget::searchCompleted, this, [this]
            (const QVector<Book>& results){
        ResultPage->fillResults(results);

        fadeToPage(Page::ResultPageIndex);
    });

    //صفحه اطلاعات کتاب در صورت کلیک روی کتاب
    connect(ResultPage, &ResultWidget::bookSelected, this, [this](Book* bookptr){
        previousPageIndex = ResultPageIndex;
        BookDetailsPage->loadBook(bookptr);
        fadeToPage(BookDetailsPageIndex);
    });

    connect(ResultPage, &ResultWidget::backPrevious, this, [this](){
        fadeToPage(Page::SearchPageIndex);
    });

    //نمایش نظرات کتاب در صورت کلیک روی نظرات در صفحه اطلاعات کتاب
    connect(BookDetailsPage, &BookDetailsWidget::goToComments, this, [this](int bookID){
        CommentsPage->loadComments(bookID);
        fadeToPage(Page::CommentsPageIndex);
    });

    //برگشت به صفحه اطلاعات کتاب از بخش نظرات
    connect(CommentsPage, &CommentsWidget::backToBookDatailPage, this, [this](){
        BookDetailsPage->loadBook();
        fadeToPage(Page::BookDetailsPageIndex);
    });

    //کلیک روی کتاب های داخل سبد خرید
    connect(CartPage, &CartWidget::bookSelected, this, [this](Book* bookptr){
        previousPageIndex = CartPageIndex;
        BookDetailsPage->loadBook(bookptr);
        fadeToPage(BookDetailsPageIndex);
    });

    //کلیک روی کتاب های داخل نشان شده ها
    connect(LibraryPage, &LibraryWidget::bookSelected, this, [this](Book* bookptr){
        previousPageIndex = LibraryPageIndex;
        BookDetailsPage->loadBook(bookptr);
        fadeToPage(BookDetailsPageIndex);
    });

    //رفتن به پی دی اف با کلیک روی مطالعه کتاب
    connect(BookDetailsPage, &BookDetailsWidget::goToPDF, this, [this](int bookID){
        prevPagePDF = Page::BookDetailsPageIndex;
        BookReaderPage->loadBook(bookID);
        fadeToPage(Page::BookReaderPageIndex);
    });

    //برگشت از صفحه پی دی اف
    connect(BookReaderPage, &BookReaderWidget::back, this, [this](){
       fadeToPage(prevPagePDF);
    });

    //رفتن به پی دی اف از طریق قفسه ها و کتاب های من
    connect(LibraryPage, &LibraryWidget::goToPDF, this, [this](int bookID){
        prevPagePDF = Page::LibraryPageIndex;
        BookReaderPage->loadBook(bookID);
        fadeToPage(Page::BookReaderPageIndex);
    });

    //رفتن به تاریخچه خرید
    connect(CartPage, &CartWidget::goToHistory, this, [this](){
        fadeToPage(Page::PurchaseHistoryPageIndex);
    });

    //برگشت به سبد خرید
    connect(PurchaseHistoryPage, &PurchaseHistoryWidget::back, this, [this](){
        fadeToPage(Page::CartPageIndex);
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


void UserDashboardWidget::on_search_pushButton_clicked()
{
    fadeToPage(Page::SearchPageIndex);
}


void UserDashboardWidget::on_cart_pushButton_clicked()
{
    fadeToPage(Page::CartPageIndex);
}


void UserDashboardWidget::on_library_pushButton_clicked()
{
    fadeToPage(Page::LibraryPageIndex);
}

