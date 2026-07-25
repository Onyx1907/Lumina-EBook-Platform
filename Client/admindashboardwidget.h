#ifndef ADMINDASHBOARDWIDGET_H
#define ADMINDASHBOARDWIDGET_H

#include <QWidget>
#include "admin.h"
#include "adminbookswidget.h"

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

private:
    Ui::AdminDashboardWidget *ui;

    void fadeToPage(int pageIndex);

    AdminBooksWidget *AdminBooksPage;

    enum Page{
        AdminBooksPageIndex = 0
    };
};

#endif // ADMINDASHBOARDWIDGET_H
