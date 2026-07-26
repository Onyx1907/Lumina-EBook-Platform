#ifndef SHAREDBOOKCARD_H
#define SHAREDBOOKCARD_H

#include <QWidget>
#include <QJsonObject>
#include "book.h"

namespace Ui {
class SharedBookCard;
}

class SharedBookCard : public QWidget
{
    Q_OBJECT

public:
    explicit SharedBookCard(const QJsonObject& obj, QWidget *parent = nullptr, bool is_admin = false);
    ~SharedBookCard();

    void updatePrice(double discount);
    int getBookID();

signals:
    void editRequested(int bookID);
    void deleteRequested(int bookID);
    void discountRequested(int bookID, double discount);
    void changeActiveRequested(int bookID, bool isActive);
    void goToPDF(QString path);
    void goToComments(int bookID);

private slots:
    void on_edit_pushButton_clicked();

    void on_discount_pushButton_clicked();

    void on_active_checkBox_toggled(bool checked);

    void on_study_pushButton_clicked();

    void on_star_clicked();

private:
    Ui::SharedBookCard *ui;
    Book book;
    bool isActive;
    QString PDFpath;

    bool isAdmin = false;
};

#endif // SHAREDBOOKCARD_H
