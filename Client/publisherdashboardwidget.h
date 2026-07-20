#ifndef PUBLISHERDASHBOARDWIDGET_H
#define PUBLISHERDASHBOARDWIDGET_H

#include <QWidget>
#include "publisher.h"
#include "profilewidget.h"
#include "publisherbookwidget.h"
#include "editbookwidget.h"

namespace Ui {
class PublisherDashboardWidget;
}

class PublisherDashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherDashboardWidget(Publisher *cur_user, QWidget *parent = nullptr);
    ~PublisherDashboardWidget();

private slots:
    void on_profile_pushButton_clicked();

    void on_books_pushButton_clicked();

private:
    Ui::PublisherDashboardWidget *ui;

    void fadeToPage(int pageIndex);

    Publisher *publisher;

    ProfileWidget *ProfilePage;
    PublisherBookWidget* PublisherBookPage;
    EditBookWidget *EditBookPage;

    enum Page{
        ProfilePageIndex = 0,
        PublisherBookPageIndex = 1,
        EditBookPageIndex = 2
    };
};

#endif // PUBLISHERDASHBOARDWIDGET_H
