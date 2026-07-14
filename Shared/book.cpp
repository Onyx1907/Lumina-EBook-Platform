#include "book.h"

Book::Book(int id, QString title, QString author, QString publisher, BookGenre genre,
           QString coverImagePath, double price, double discountPercentage)
    : id(id), title(title), author(author), publisher(publisher), genre(genre),
    price(price), discountPercentage(discountPercentage) , coverImagePath(coverImagePath){}

int Book::getId() const { return id; }

QString Book::getTitle() const { return title; }

QString Book::getAuthor() const { return author; }

BookGenre Book::getGenre() const { return genre; }

double Book::getFinalPrice() const { return price * (1.0 - (discountPercentage / 100.0)); }

QString Book::getCoverImagePath() const { return coverImagePath; }

QString Book::getPublisher() const{ return publisher; }

double Book::getPrice() const{ return price; }

double Book::getDiscountPercentage() const{ return discountPercentage; }
