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
    explicit BookDetailsWidget(int ID, QWidget *parent = nullptr);
    void loadBook(Book *book = nullptr);
    ~BookDetailsWidget();

signals:
    void backPrevious();
    void goToComments(int bookID);
    void goToPDF(int bookID);

private slots:
    void on_back_pushButton_clicked();

    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_addCart_pushButton_clicked();

    void on_removeCart_pushButton_clicked();

    void on_comments_pushButton_clicked();

    void on_saveBook_pushButton_clicked();

    void on_savedBook_pushButton_clicked();

    void on_study_pushButton_clicked();

private:
    Ui::BookDetailsWidget *ui;
    int userID;
    Book* cur_book = nullptr;
};

#endif // BOOKDETAILSWIDGET_H
