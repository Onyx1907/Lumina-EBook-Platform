#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterWidget; }
QT_END_NAMESPACE

class RegisterWidget : public QWidget {
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

    void enableFormWithError(const QString& errorMsg);

signals:
    //  سیگنال‌هایی که به مِین‌ویندو می‌گویند کاربر می‌خواهد جابجا شود
    void goToLoginRequested();


private slots:
    void on_back_pushButton_clicked();

    void on_register_pushButton_clicked();

private:
    Ui::RegisterWidget *ui;
};

#endif // REGISTERWIDGET_H
