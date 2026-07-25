#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <regularuser.h>
#include <publisher.h>

namespace Ui {
class ProfileWidget;
}

class ProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileWidget(User *cur_user, QWidget *parent = nullptr);
    ~ProfileWidget();

    void loadProfile();

signals:
    void goToGenreSelectionPage();

private slots:
    void on_changeGenres_pushButton_clicked();

    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_submitProfile_pushButton_clicked();

    void on_submitPass_pushButton_clicked();

private:
    Ui::ProfileWidget *ui;

    User *user;
    QString new_username = "";

    bool is_publisher = false;

};

#endif // PROFILEWIDGET_H
