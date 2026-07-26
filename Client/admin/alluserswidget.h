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

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::AllUsersWidget *ui;
    int pendingScrollValue = 0;

    void fillResults(const QJsonObject& data);
};

#endif // ALLUSERSWIDGET_H
