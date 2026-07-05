#ifndef USERHOMEWIDGET_H
#define USERHOMEWIDGET_H

#include <QWidget>
#include <QListWidget>


namespace Ui {
class UserHomeWidget;
}

class UserHomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserHomeWidget(QString cur_username, QWidget *parent = nullptr);
    ~UserHomeWidget();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void processNetworkData(const QString& action, const QJsonObject& data);

private:
    Ui::UserHomeWidget *ui;

    QString username;

    void parseAndFillList(const QJsonObject &data, QListWidget *targetList);
};

#endif // USERHOMEWIDGET_H
