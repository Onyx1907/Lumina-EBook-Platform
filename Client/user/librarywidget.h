#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "book.h"


struct Shelf{
    int id;
    QString name;
};


namespace Ui {
class LibraryWidget;
}

class LibraryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryWidget(int userID, QWidget *parent = nullptr);
    ~LibraryWidget();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void on_tabWidget_currentChanged(int index);

    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_bookCard_clicked(Book* bookPtr);

    void on_shelf_clicked(QListWidgetItem* item);

    void on_pushButton_clicked();

private:
    Ui::LibraryWidget *ui;
    int m_userID;
    QList <Shelf> m_shelves;
    int m_active_shelfID;

    void requestPurchasedBooks();
    void requestSavedBooks();
    void requestShelves();
    void requestShelfBooks(int shelfID);

    void sendRenameShelfRequest(int shelfId, QString newName);
    void sendDeleteShelfRequest(int shelfId);
    void sendCreateShelfRequest(QString shelfName);
    void sendAddBookToShelfRequest(int bookId, int shelfId);
    void sendMoveBookRequest(int bookId, int fromShelfId, int toShelfId);

    void onShelfContextMenuRequested(const QPoint &pos);

    void handleGetPurchasedBooks(const QJsonObject& response);
    void handleGetSavedBooks(const QJsonObject& response);
    void handleGetShelves(const QJsonObject& response);
    void handleGetShelfBooks(const QJsonObject& response);
};

#endif // LIBRARYWIDGET_H
