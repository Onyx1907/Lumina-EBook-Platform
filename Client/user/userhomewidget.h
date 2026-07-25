#ifndef USERHOMEWIDGET_H
#define USERHOMEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "book.h"


namespace Ui {
class UserHomeWidget;
}

class UserHomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserHomeWidget(int ID, QWidget *parent = nullptr);
    ~UserHomeWidget();

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void bookSelected(Book* book);

private slots:
    void processNetworkData(const QString& action, const QJsonObject& data);

private:
    Ui::UserHomeWidget *ui;

    int userID;

    void parseAndFillList(const QJsonObject &data, QListWidget *targetList);
};

#endif // USERHOMEWIDGET_H
