#ifndef CARTWIDGET_H
#define CARTWIDGET_H

#include <QWidget>

namespace Ui {
class CartWidget;
}

class CartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CartWidget(int userID, QWidget *parent = nullptr);
    ~CartWidget();

protected:
    void showEvent(QShowEvent *event) override;

private slots:

    void processNetworkData(const QString& action, const QJsonObject& data);

    void onBookRemoveRequested(int bookId);

    void on_pay_pushButton_clicked();

private:
    Ui::CartWidget *ui;
    int m_userID;
    double m_finalPrice;

    void loadCartFromServer();

    void handleGetCartResponse(const QJsonObject& response);
};

#endif // CARTWIDGET_H
