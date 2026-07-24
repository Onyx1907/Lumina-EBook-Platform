#ifndef INOVICECARD_H
#define INOVICECARD_H

#include <QWidget>

namespace Ui {
class InoviceCard;
}

class InoviceCard : public QWidget
{
    Q_OBJECT

public:
    explicit InoviceCard(int bookID, QString date, QString title,
                               QString author, int price, QWidget *parent = nullptr);
    ~InoviceCard();

private:
    Ui::InoviceCard *ui;

    int m_bookID;
};

#endif // INOVICECARD_H
