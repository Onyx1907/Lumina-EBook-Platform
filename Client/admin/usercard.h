#ifndef USERCARD_H
#define USERCARD_H

#include <QWidget>

namespace Ui {
class UserCard;
}

class UserCard : public QWidget
{
    Q_OBJECT

public:
    explicit UserCard(const QJsonObject& user, QWidget *parent = nullptr);
    ~UserCard();

signals:
    void deleteRequested(int userID);
    void changeBlockedState(int userID, bool isBlocked);
    void detailsRequested(int userID);

private slots:
    void on_delete_pushButton_clicked();

    void on_block_checkBox_toggled(bool checked);

    void on_back_pushButton_clicked();

private:
    Ui::UserCard *ui;

    int m_userID;
};

#endif // USERCARD_H
