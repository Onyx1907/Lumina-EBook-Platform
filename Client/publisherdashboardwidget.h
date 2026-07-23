#ifndef PUBLISHERDASHBOARDWIDGET_H
#define PUBLISHERDASHBOARDWIDGET_H

#include <QWidget>
#include "publisher.h"
#include "profilewidget.h"
#include "publisherbookwidget.h"
#include "editbookwidget.h"
#include "bookreaderwidget.h"
#include "publisherstatswidget.h"

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

    void on_home_pushButton_clicked();

private:
    Ui::PublisherDashboardWidget *ui;

    void fadeToPage(int pageIndex);

    Publisher *publisher;

    PublisherStatsWidget *PublisherStatsPage;
    ProfileWidget *ProfilePage;
    PublisherBookWidget* PublisherBookPage;
    EditBookWidget *EditBookPage;
    BookReaderWidget *BookReaderPage;

    enum Page{
        PublisherStatsPageIndex = 0,
        ProfilePageIndex = 1,
        PublisherBookPageIndex = 2,
        EditBookPageIndex = 3,
        BookReaderPageIndex = 4
    };
};

#endif // PUBLISHERDASHBOARDWIDGET_H
