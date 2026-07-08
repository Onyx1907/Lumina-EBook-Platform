#ifndef USERDASHBOARDWIDGET_H
#define USERDASHBOARDWIDGET_H

#include <QWidget>
#include "regularuser.h"
#include "genreselectionwidget.h"
#include "userhomewidget.h"
#include "profilewidget.h"


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

private:
    Ui::UserDashboardWidget *ui;
    RegularUser *user;

    GenreSelectionWidget  *GenreSelectionPage;
    UserHomeWidget *UserHomePage;
    ProfileWidget* ProfilePage;

    void fadeToPage(int pageIndex);

    enum Page{
        GenreSelectionPageIndex = 0,
        UserHomePageIndex = 1,
        ProfilePageIndex = 2
    };

};




#endif // USERDASHBOARDWIDGET_H
