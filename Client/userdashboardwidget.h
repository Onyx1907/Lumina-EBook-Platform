#ifndef USERDASHBOARDWIDGET_H
#define USERDASHBOARDWIDGET_H

#include <QWidget>
#include "regularuser.h"
#include "genreselectionwidget.h"
#include "userhomewidget.h"
#include "profilewidget.h"
#include "bookdetailswidget.h"
#include "searchwidget.h"
#include "resultwidget.h"
#include "commentswidget.h"
#include "cartwidget.h"
#include "librarywidget.h"
#include "bookreaderwidget.h"
#include "purchasehistorywidget.h"
#include "notificationswidget.h"


namespace Ui {
class UserDashboardWidget;
}

class UserDashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserDashboardWidget(RegularUser* cur_user, bool is_first_login, QWidget *parent = nullptr);
    ~UserDashboardWidget();

protected:
    //void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_home_pushButton_clicked();

    void on_profile_pushButton_clicked();

    void on_search_pushButton_clicked();

    void on_cart_pushButton_clicked();

    void on_library_pushButton_clicked();

private:
    Ui::UserDashboardWidget *ui;
    RegularUser *user;

    GenreSelectionWidget  *GenreSelectionPage;
    UserHomeWidget *UserHomePage;
    ProfileWidget* ProfilePage;
    BookDetailsWidget* BookDetailsPage;
    SearchWidget* SearchPage;
    ResultWidget* ResultPage;
    CommentsWidget* CommentsPage;
    CartWidget* CartPage;
    LibraryWidget* LibraryPage;
    BookReaderWidget* BookReaderPage;
    PurchaseHistoryWidget * PurchaseHistoryPage;
    NotificationsWidget *NotificationsPage;

    int previousPageIndex = 0;
    int prevPagePDF = 0;
    int prevNotifs = 0;

    void fadeToPage(int pageIndex);

    enum Page{
        GenreSelectionPageIndex = 0,
        UserHomePageIndex = 1,
        ProfilePageIndex = 2,
        BookDetailsPageIndex = 3,
        SearchPageIndex = 4,
        ResultPageIndex = 5,
        CommentsPageIndex = 6,
        CartPageIndex = 7,
        LibraryPageIndex = 8,
        BookReaderPageIndex = 9,
        PurchaseHistoryPageIndex = 10,
        NotificationsPageIndex = 11
    };


    void testSearchLayout();

};




#endif // USERDASHBOARDWIDGET_H
