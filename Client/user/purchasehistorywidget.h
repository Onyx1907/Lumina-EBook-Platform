#ifndef PURCHASEHISTORYWIDGET_H
#define PURCHASEHISTORYWIDGET_H

#include <QWidget>

namespace Ui {
class PurchaseHistoryWidget;
}

class PurchaseHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PurchaseHistoryWidget(int userID, QWidget *parent = nullptr);
    ~PurchaseHistoryWidget();

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void back();

private slots:

    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_back_pushButton_clicked();

private:
    Ui::PurchaseHistoryWidget *ui;
    int m_userID;
};

#endif // PURCHASEHISTORYWIDGET_H
