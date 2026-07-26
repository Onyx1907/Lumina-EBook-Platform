#ifndef USERDETAILSDIALOG_H
#define USERDETAILSDIALOG_H

#include <QDialog>

namespace Ui {
class UserDetailsDialog;
}

class UserDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDetailsDialog(const QJsonObject& user, QWidget *parent = nullptr);
    ~UserDetailsDialog();

private slots:
    void on_back_pushButton_clicked();

private:
    Ui::UserDetailsDialog *ui;
};

#endif // USERDETAILSDIALOG_H
