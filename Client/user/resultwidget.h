#ifndef RESULTWIDGET_H
#define RESULTWIDGET_H

#include <QWidget>
#include "book.h"

namespace Ui {
class ResultWidget;
}

class ResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResultWidget(QWidget *parent = nullptr);
    ~ResultWidget();

    void fillResults(const QVector<Book>& booksList);

signals:
    void bookSelected(Book* bookptr);
    void backPrevious();

private slots:
    void on_back_pushButton_clicked();

private:
    Ui::ResultWidget *ui;
};

#endif // RESULTWIDGET_H
