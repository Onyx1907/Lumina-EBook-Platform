#ifndef BOOKINFOCARD_H
#define BOOKINFOCARD_H

#include <QWidget>
#include "book.h"
#include <QPushButton>

namespace Ui {
class BookInfoCard;
}

class BookInfoCard : public QWidget
{
    Q_OBJECT

public:
    explicit BookInfoCard(Book b, QWidget *parent = nullptr);
    ~BookInfoCard();

signals:
    void clicked(Book* bookPtr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::BookInfoCard *ui;
    Book book;


    QPushButton* book_pushbutton = nullptr;
};

#endif // BOOKINFOCARD_H
