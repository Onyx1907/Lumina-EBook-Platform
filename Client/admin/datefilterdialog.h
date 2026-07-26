#ifndef DATEFILTERDIALOG_H
#define DATEFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class DateFilterDialog;
}

class DateFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DateFilterDialog(QWidget *parent = nullptr);
    ~DateFilterDialog();

    QString selectedDate() const;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::DateFilterDialog *ui;

    QString m_selectedDate = "";
};

#endif // DATEFILTERDIALOG_H
