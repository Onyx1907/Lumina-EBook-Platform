#ifndef BOOKCARD_H
#define BOOKCARD_H

#include <QWidget>
#include <QPushButton>
#include <QResizeEvent>
#include "book.h"

namespace Ui {
class BookCard;
}

class BookCard : public QWidget
{
    Q_OBJECT

public:
    explicit BookCard(Book book, QWidget *parent = nullptr);
    ~BookCard();

signals:
    void clicked(Book* bookPtr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::BookCard *ui;
    Book book;

    QPushButton* book_pushbutton = nullptr;
};

#endif // BOOKCARD_H
