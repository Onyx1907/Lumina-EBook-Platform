#ifndef BOOKDETAILSWIDGET_H
#define BOOKDETAILSWIDGET_H

#include <QWidget>
#include "book.h"

namespace Ui {
class BookDetailsWidget;
}

class BookDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookDetailsWidget(QWidget *parent = nullptr);
    void loadBook(Book *book);
    ~BookDetailsWidget();

signals:
    void backPrevious();

private slots:
    void on_back_pushButton_clicked();

private:
    Ui::BookDetailsWidget *ui;
};

#endif // BOOKDETAILSWIDGET_H
