#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include "constants.h"

class Book {
private:
    int id;
    QString title;
    QString author;
    int publisherId;
    BookGenre genre;
    QString description;
    double price;
    double discountPercentage;
    QString pdfPath;
    QString coverImagePath;

public:
    Book(int id, QString title, QString author, int publisherId, BookGenre genre,
         QString coverImagePath, double price, double discountPercentage = 0.0);

    int getId() const;
    QString getTitle() const;
    QString getAuthor() const;
    BookGenre getGenre() const;
    double getFinalPrice() const;

    QString getCoverImagePath() const;
};

#endif // BOOK_H
