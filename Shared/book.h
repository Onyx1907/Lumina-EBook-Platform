#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include "constants.h"

class Book {
private:
    int id;
    QString title;
    QString author;
    QString publisher;
    BookGenre genre;
    QString description;
    double price;
    double discountPercentage;
    QString pdfPath;
    QString coverImagePath;
    double rating;

public:
    Book(int id, QString title, QString author, QString publisher, BookGenre genre,
         QString coverImagePath, double price, double discountPercentage = 0.0, double rating = -1);

    int getId() const;
    QString getTitle() const;
    QString getAuthor() const;
    BookGenre getGenre() const;
    double getFinalPrice() const;
    QString getPublisher() const;
    double getPrice() const;
    double getDiscountPercentage() const;

    QString getCoverImagePath() const;

    double getRating();
    void setPrice(double price, double discount);
};

#endif // BOOK_H
