#ifndef ADMINDASHBOARDWIDGET_H
#define ADMINDASHBOARDWIDGET_H

#include <QWidget>
#include "admin.h"
#include "adminbookswidget.h"
#include "editbookwidget.h"
#include "bookreaderwidget.h"
#include "commentswidget.h"
#include "alluserswidget.h"

namespace Ui {
class AdminDashboardWidget;
}

class AdminDashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminDashboardWidget(Admin *admin, QWidget *parent = nullptr);
    ~AdminDashboardWidget();

private slots:
    void on_books_pushButton_clicked();

    void on_users_pushButton_clicked();

private:
    Ui::AdminDashboardWidget *ui;

    void fadeToPage(int pageIndex);

    AdminBooksWidget *AdminBooksPage;
    EditBookWidget *EditBookPage;
    BookReaderWidget *BookReaderPage;
    CommentsWidget *CommentsPage;
    AllUsersWidget *AllUsersPage;

    enum Page{
        AdminBooksPageIndex = 0,
        EditBookPageIndex = 1,
        BookReaderPageIndex = 2,
        CommentsPageIndex = 3,
        AllUsersPageIndex = 4
    };
};

#endif // ADMINDASHBOARDWIDGET_H
