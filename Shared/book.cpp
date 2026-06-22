#include "book.h"

Book::Book(int id, QString title, QString author, int publisherId, BookGenre genre,
           double price, QString pdfPath, QString coverImagePath, double discountPercentage)
    : id(id), title(title), author(author), publisherId(publisherId), genre(genre),
    price(price), pdfPath(pdfPath), coverImagePath(coverImagePath), discountPercentage(discountPercentage) {}

int Book::getId() const { return id; }

QString Book::getTitle() const { return title; }

QString Book::getAuthor() const { return author; }

BookGenre Book::getGenre() const { return genre; }

double Book::getFinalPrice() const { return price * (1.0 - (discountPercentage / 100.0)); }
