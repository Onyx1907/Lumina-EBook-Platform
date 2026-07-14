#include "commentitemwidget.h"
#include "ui_commentitemwidget.h"
#include <QMessageBox>

CommentItemWidget::CommentItemWidget(const Comment& comment, int currentUserId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommentItemWidget),
    m_comment(comment)
{
    ui->setupUi(this);

    // ۱. لود کردن دیتای کامنت روی المان‌ها با اسامی جدیدی که گذاشتی
    ui->username_label->setText("@" + m_comment.getUsername());
    ui->text_label->setText(m_comment.getText());
    ui->date_label->setText(m_comment.formattedDate());

    ui->rating_label->setText(QString("⭐ %1 / 5").arg(m_comment.getRating()));

    // ۲. بررسی مالکیت کامنت برای نمایش یا مخفی کردن دکمه‌ها
    if (m_comment.getUserID() == currentUserId) {
        ui->edit_pushButton->show();
        ui->delete_pushButton->show();
    } else {
        ui->edit_pushButton->hide();
        ui->delete_pushButton->hide();
    }

    // ۳. اتصال دکمه‌های جدید به سیگنال‌ها برای ارسال به صفحه اصلی
    connect(ui->delete_pushButton, &QPushButton::clicked, this, [this]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("حذف نظر");
        msgBox.setText("آیا از حذف این نظر مطمئن هستید؟");
        msgBox.setIcon(QMessageBox::Question);

        // اضافه کردن دکمه‌های استاندارد به زبان فارسی
        QPushButton *yesButton = msgBox.addButton("بله، حذف شود", QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton("خیر", QMessageBox::NoRole);
        msgBox.setDefaultButton(noButton);

        // 🎨 جادوی QSS برای شیک کردن کل بدنه و دکمه‌های QMessageBox
        msgBox.setStyleSheet(
            "QMessageBox {"
            "   background-color: #1A1512;" // پس‌زمینه تیره نسکافه‌ای
            "   border: 1px solid #C19A6B;" // مرز طلایی/نسکافه‌ای ملایم
            "   border-radius: 12px;"
            "}"
            "QLabel {"
            "   color: #E6D7C3;" // رنگ متن کرم روشن
            "   font-family: 'Segoe UI', 'Tahoma';"
            "   font-size: 13px;"
            "   padding: 10px;"
            "}"
            "QPushButton {"
            "   background-color: rgba(193, 154, 107, 0.15);"
            "   border: 1px solid rgba(193, 154, 107, 0.4);"
            "   border-radius: 6px;"
            "   color: #E6D7C3;"
            "   font-family: 'Segoe UI', 'Tahoma';"
            "   font-size: 11px;"
            "   min-width: 80px;"
            "   padding: 6px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #C19A6B;"
            "   color: #1E1914;" // متن تیره موقع هاور برای خوانایی
            "   font-weight: bold;"
            "}"
            );

        msgBox.exec();

        // بررسی اینکه کاربر کدام دکمه را کلیک کرده است
        if (msgBox.clickedButton() == yesButton) {
            emit deleteRequested(m_comment.getId());
        }
    });

    connect(ui->edit_pushButton, &QPushButton::clicked, this, [this]() {
        emit editRequested(m_comment);
    });
}

CommentItemWidget::~CommentItemWidget() {
    delete ui;
}
