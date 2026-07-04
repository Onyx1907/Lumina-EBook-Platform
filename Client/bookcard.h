#ifndef BOOKCARD_H
#define BOOKCARD_H

#include <QWidget>

namespace Ui {
class BookCard;
}

class BookCard : public QWidget
{
    Q_OBJECT

public:
    explicit BookCard(QWidget *parent = nullptr);
    ~BookCard();

private:
    Ui::BookCard *ui;
};

#endif // BOOKCARD_H
