#ifndef ADMINBOOKSWIDGET_H
#define ADMINBOOKSWIDGET_H

#include <QWidget>

namespace Ui {
class AdminBooksWidget;
}

class AdminBooksWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminBooksWidget(QWidget *parent = nullptr);
    ~AdminBooksWidget();

signals:
    void editBook(int bookID);
    void PDFreader(QString path);
    void goToCommentsPage(int bookID);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void processNetworkData(const QString& action, const QJsonObject& data);

    void deleteBookRequested(int bookID);

private:
    Ui::AdminBooksWidget *ui;

    void loadBooks();

    void handleGetBooks(const QJsonObject& data);

    int pendingScrollValue = 0;
};

#endif // ADMINBOOKSWIDGET_H
