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

    enum CardMode{
        Normal,
        Cart
    };

    void setCardMode(CardMode mode = CardMode::Normal);
    void updatePrice(double price, double discount);
    int getBookID();

signals:
    void clicked(Book* bookPtr);
    void removeRequested(int bookID);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_delete_pushButton_clicked();

private:
    Ui::BookInfoCard *ui;
    Book book;


    QPushButton* book_pushbutton = nullptr;
};

#endif // BOOKINFOCARD_H
