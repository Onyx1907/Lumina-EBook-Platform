#ifndef EDITBOOKWIDGET_H
#define EDITBOOKWIDGET_H

#include <QWidget>

namespace Ui {
class EditBookWidget;
}

class EditBookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditBookWidget(QWidget *parent = nullptr, int publisherID = -1);
    ~EditBookWidget();

    void loadEditBook(int bookID = -1);

signals:
    void back();

private slots:
    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_chooseCover_pushButton_clicked();

    void on_choosePDF_pushButton_clicked();

    void on_submit_pushButton_clicked();

    void on_back_pushButton_clicked();

private:
    Ui::EditBookWidget *ui;
    int m_bookID = -1;
    int m_publisherID = -1;
    QString coverPath = "";
    QString filePath = "";
};

#endif // EDITBOOKWIDGET_H
