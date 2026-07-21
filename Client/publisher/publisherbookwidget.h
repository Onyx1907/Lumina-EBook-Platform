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

protected:
    void showEvent(QShowEvent *event) override;

private slots:
        void processNetworkData(const QString& action, const QJsonObject& data);

        void on_back_pushButton_clicked();

        void checkIsActiveRequested(int bookID, bool isActive);

    private:
    Ui::PublisherBookWidget *ui;
    int m_publisherID;

    void loadBooks();
    void handleGetBooks(const QJsonObject& data);
};

#endif // PUBLISHERBOOKWIDGET_H
