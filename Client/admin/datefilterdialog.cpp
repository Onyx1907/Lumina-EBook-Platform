#include "datefilterdialog.h"
#include "ui_datefilterdialog.h"
#include <QDate>

DateFilterDialog::DateFilterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DateFilterDialog)
{
    ui->setupUi(this);

    // ui->buttonBox->button(QDialogButtonBox::Ok)->setText("تأیید");

    // ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("لغو");
}

DateFilterDialog::~DateFilterDialog()
{
    delete ui;
}

void DateFilterDialog::on_buttonBox_accepted()
{
    QDate d = ui->calendarWidget->selectedDate();

    if(ui->monthRadio->isChecked())
        m_selectedDate = d.toString("yyyy-MM");
    else
        m_selectedDate = d.toString("yyyy-MM-dd");

    QDialog::accept();
}


void DateFilterDialog::on_buttonBox_rejected()
{
    QDialog::reject();
}

QString DateFilterDialog::selectedDate() const{
    return m_selectedDate;
}
