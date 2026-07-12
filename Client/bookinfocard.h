#ifndef BOOKINFOCARD_H
#define BOOKINFOCARD_H

#include <QWidget>

namespace Ui {
class BookInfoCard;
}

class BookInfoCard : public QWidget
{
    Q_OBJECT

public:
    explicit BookInfoCard(QWidget *parent = nullptr);
    ~BookInfoCard();

private:
    Ui::BookInfoCard *ui;
};

#endif // BOOKINFOCARD_H
