#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QWidget>

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

private:
    Ui::LibraryWidget *ui;
    int m_userID;

    void requestPurchasedBooks();
    void requestSavedBooks();
    void requestShelves();

    void handleGetPurchasedBooks(const QJsonObject& response);
    void handleGetSavedBooks(const QJsonObject& response);
    // void handleGetShelves();
};

#endif // LIBRARYWIDGET_H
