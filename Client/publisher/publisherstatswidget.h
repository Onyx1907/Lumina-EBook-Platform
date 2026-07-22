#ifndef PUBLISHERSTATSWIDGET_H
#define PUBLISHERSTATSWIDGET_H

#include <QWidget>

namespace Ui {
class PublisherStatsWidget;
}

class PublisherStatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherStatsWidget(int publisherID, QWidget *parent = nullptr);
    ~PublisherStatsWidget();

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void goToBooks();

private slots:
    void on_books_pushButton_clicked();
    void processNetworkData(const QString& action, const QJsonObject& data);

private:
    Ui::PublisherStatsWidget *ui;
    int m_publisherID;
};

#endif // PUBLISHERSTATSWIDGET_H
