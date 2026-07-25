#ifndef PUBLISHERBOOKWIDGET_H
#define PUBLISHERBOOKWIDGET_H

#include <QWidget>
#include "sharedbookcard.h"

namespace Ui {
class PublisherBookWidget;
}

class PublisherBookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherBookWidget(int publisherID ,QWidget *parent = nullptr);
    ~PublisherBookWidget();

signals:
    void addBook();
    void editBook(int bookID);
    void PDFreader(QString path);
    void goToCommentsPage(int bookID);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
        void processNetworkData(const QString& action, const QJsonObject& data);

        void on_back_pushButton_clicked();

        void checkIsActiveRequested(int bookID, bool isActive);

        void deleteBookRequested(int bookID);

        void setDiscountRequested(int bookID, double percent);

    private:
    Ui::PublisherBookWidget *ui;
    int m_publisherID;

    int m_bookID;
    int m_discount;

    void loadBooks();
    void handleGetBooks(const QJsonObject& data);
};

#endif // PUBLISHERBOOKWIDGET_H
