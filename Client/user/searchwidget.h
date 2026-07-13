#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include "book.h"

namespace Ui {
class SearchWidget;
}

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = nullptr);
    ~SearchWidget();

private slots:
    void on_educational_pushButton_clicked();

    void on_biography_pushButton_clicked();

    void on_scifi_pushButton_clicked();

    void on_psychology_pushButton_clicked();

    void on_history_pushButton_clicked();

    void on_ficition_pushButton_clicked();

    void on_search_pushButton_clicked();


    void processNetworkData(const QString& action, const QJsonObject& data);

signals:
    void searchCompleted(const QVector<Book>& results);

private:
    Ui::SearchWidget *ui;
};

#endif // SEARCHWIDGET_H
