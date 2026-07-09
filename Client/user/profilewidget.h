#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <regularuser.h>

namespace Ui {
class ProfileWidget;
}

class ProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileWidget(RegularUser *cur_user, QWidget *parent = nullptr);
    ~ProfileWidget();

    void loadProfile();

signals:
    void goToGenreSelectionPage();

private slots:
    void on_changeGenres_pushButton_clicked();

    void processNetworkData(const QString& action, const QJsonObject& data);

private:
    Ui::ProfileWidget *ui;

    RegularUser *user;

};

#endif // PROFILEWIDGET_H
