#ifndef ALLUSERSWIDGET_H
#define ALLUSERSWIDGET_H

#include <QWidget>

namespace Ui {
class AllUsersWidget;
}

class AllUsersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AllUsersWidget(QWidget *parent = nullptr);
    ~AllUsersWidget();

    void loadUsers();

private slots:
    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_date_toolButton_clicked();

    void on_clear_toolButton_clicked();

    void on_search_pushButton_clicked();

    void on_allUsers_pushButton_clicked();


private:
    Ui::AllUsersWidget *ui;
    int pendingScrollValue = 0;
    QString registerDate;

    void fillResults(const QJsonObject& data, bool is_new = false);
};

#endif // ALLUSERSWIDGET_H
