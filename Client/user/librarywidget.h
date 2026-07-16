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

private:
    Ui::LibraryWidget *ui;
    int m_userID;

    void requestPurchasedBooks();
    void requestSavedBooks();
    void requestShelves();
};

#endif // LIBRARYWIDGET_H
