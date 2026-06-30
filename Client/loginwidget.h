#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWidget; }
QT_END_NAMESPACE

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    //  سیگنال‌هایی که به مِین‌ویندو می‌گویند کاربر می‌خواهد جابجا شود
    void goToRegisterRequested();
    void goToForgotPasswordRequested();


private slots:
    void on_register_pushButton_clicked();

    void on_forget_pushButton_clicked();

    void on_login_pushButton_clicked();

    void processNetworkData(const QString& action, const QJsonObject& data);

private:
    Ui::LoginWidget *ui;

    void enableFormWithError(const QString& errorMsg);

};

#endif // LOGINWIDGET_H
