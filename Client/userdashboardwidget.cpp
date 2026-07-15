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

    ui->stackedWidget->addWidget(GenreSelectionPage);
    ui->stackedWidget->addWidget(UserHomePage);
    ui->stackedWidget->addWidget(ProfilePage);
    ui->stackedWidget->addWidget(BookDetailsPage);
    ui->stackedWidget->addWidget(SearchPage);
    ui->stackedWidget->addWidget(ResultPage);
    ui->stackedWidget->addWidget(CommentsPage);
    ui->stackedWidget->addWidget(CartPage);

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
        fadeToPage(Page::BookDetailsPageIndex);
    });

    //کلیک روی کتاب های داخل سبد خرید
    connect(CartPage, &CartWidget::bookSelected, this, [this](Book* bookptr){
        previousPageIndex = CartPageIndex;
        BookDetailsPage->loadBook(bookptr);
        fadeToPage(BookDetailsPageIndex);
    });

    connect(ResultPage, &ResultWidget::backPrevious, this, [this](){
        fadeToPage(Page::CartPageIndex);
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

    //تست
    testSearchLayout();
}


//تست گرافیک صفحه نتایج

void UserDashboardWidget::testSearchLayout() {
    QVector<Book> fakeBooks;

    // ۱. کتاب اول: ژانر تخیلی (Fiction) با ۱۰ درصد تخفیف
    fakeBooks.append(Book(
        10,
        "شاهنامه فردوسی",
        "ابوالقاسم فردوسی",
        "انتشارات طوس",
        BookGenre::Fiction,
        ":/images/covers/shahnameh.jpg",
        250000.0,
        10.0
        ));

    // ۲. کتاب دوم: ژانر تاریخ (History) بدون تخفیف
    fakeBooks.append(Book(
        11,
        "تاریخ بیهقی",
        "ابوالفضل بیهقی",
        "انتشارات علمی و فرهنگی",
        BookGenre::History,
        ":/images/covers/bayhaqi.jpg",
        180000.0,
        0.0
        ));

    // ۳. کتاب سوم: علمی تخیلی (SciFi) با ۳۰ درصد تخفیف
    fakeBooks.append(Book(
        12,
        "رؤیای مریخ",
        "آرتور سی کلارک",
        "انتشارات نگاه",
        BookGenre::SciFi,
        ":/images/covers/marsexpress.jpg",
        150000.0,
        30.0
        ));

    // ۴. کتاب چهارم: روانشناسی (Psychology)
    fakeBooks.append(Book(
        13,
        "انسان در جستجوی معنا",
        "ویکتور فرانکل",
        "انتشارات چشمه",
        BookGenre::Psychology,
        ":/images/covers/meaning.jpg",
        950000.0,
        0.0
        ));

    // ۵. کتاب پنجم: آموزشی (Educational)
    fakeBooks.append(Book(
        14,
        "تفکر سریع و کند",
        "دانیال کانمن",
        "انتشارات امیرکبیر",
        BookGenre::Educational,
        ":/images/covers/thinking.jpg",
        450000.0,
        15.0
        ));

    // تزریق مستقیم وکتور فیک به صفحه ریزالت
    ResultPage->fillResults(fakeBooks);

    // انتقال موقت به صفحه ریزالت برای تست چشمی ظاهر نسکافه‌ای شیشه‌ای
    fadeToPage(Page::ResultPageIndex);
}

void UserDashboardWidget::on_cart_pushButton_clicked()
{
    fadeToPage(Page::CartPageIndex);
}

