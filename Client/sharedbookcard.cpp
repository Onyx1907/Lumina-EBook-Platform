#include "sharedbookcard.h"
#include "ui_sharedbookcard.h"
#include <QTimer>
#include "constants.h"
#include "storageutils.h"
#include <QFont>
#include <QMessageBox>
#include "clientnetworkmanager.h"
#include <QInputDialog>


SharedBookCard::SharedBookCard(const QJsonObject& obj, QWidget *parent, bool is_admin)
    : QWidget(parent), book(obj["id"].toInt(),
                        obj["title"].toString(),
                        obj["author"].toString(),
                        obj["publisher_name"].toString(), stringToGenre(obj["genre"].toString()),
                        obj["coverImagePath"].toString(),
                        obj["price"].toDouble(),
                        obj["discountPercent"].toDouble(),
                        obj["avgRating"].toDouble()),
    isActive((obj["isActive"].toInt()) == 1),
    PDFpath(obj.value("pdfPath").toString()), isAdmin(is_admin)
    , ui(new Ui::SharedBookCard)
{
    ui->setupUi(this);

    ui->tooman_label->show();

    ui->name_label->setText(book.getTitle());
    ui->author_label->setText(book.getAuthor());
    ui->description_label->setText(obj["description"].toString());
    ui->rating_label->setText(QString::number(book.getRating(), 'f', 1));
    StorageUtils::displayBookCover(book.getCoverImagePath(),
                                   ui->book_cover);


    ui->active_checkBox->blockSignals(true);
    ui->active_checkBox->setChecked(isActive);
    ui->active_checkBox->blockSignals(false);

    if(isAdmin){
        ui->discount_pushButton->hide();
        ui->active_checkBox->setEnabled(false);
        ui->rating_label->hide();
        ui->star->setText("مشاهده نظرات");
        ui->publisher_label->setText("آیدی ناشر: " + QString::number(obj["publisher_id"].toInt())
                                     + "    " + book.getPublisher());
    }
    else{
        ui->active_checkBox->setEnabled(true);
        ui->publisher_label->hide();
    }

    QString sv = genreToString(book.getGenre());

    if (sv == "Biography")
        ui->genre_label->setText("زندگینامه");
    else if (sv == "Educational")
        ui->genre_label->setText("علمی و تحصیلی");
    else if (sv == "Psychology")
        ui->genre_label->setText("روانشناسی");
    else if (sv == "Fiction")
        ui->genre_label->setText("ادبیات داستانی");
    else if (sv == "History")
        ui->genre_label->setText("تاریخی");
    else if (sv == "SciFi")
        ui->genre_label->setText("علمی تخیلی");
    else
        ui->genre_label->setText("");


    if(book.getDiscountPercentage() && book.getPrice()){
        ui->discount_label->show();
        ui->oldPrice_label->show();
        ui->discount_label->setText(QString::number(book.getDiscountPercentage(), 'f', 1) + "%");
        ui->oldPrice_label->setText(QString::number(book.getPrice(), 'f', 0));
        ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
        QFont font = ui->oldPrice_label->font();
        font.setStrikeOut(true);
        ui->oldPrice_label->setFont(font);
    }
    else{
        ui->discount_label->hide();
        ui->oldPrice_label->hide();
        ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
    }
    if(book.getPrice() == 0){
        ui->finalPrice_label->setText("رایگان");
        ui->tooman_label->hide();
    }

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &SharedBookCard::handleCheckIsActive);

    connect(ui->delete_pushButton, &QPushButton::clicked, this, [this]() {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("حذف کتاب");
        msgBox.setText("آیا از حذف این کتاب مطمئن هستید؟ امکان بازگردانی وجود ندارد");
        msgBox.setIcon(QMessageBox::Question);


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

        if (msgBox.clickedButton() == yesButton) {
            emit deleteRequested(book.getId());
        }
    });
}

SharedBookCard::~SharedBookCard()
{
    delete ui;
}


void SharedBookCard::on_edit_pushButton_clicked()
{
    emit editRequested(book.getId());
}


void SharedBookCard::on_discount_pushButton_clicked()
{
    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle("اعمال تخفیف");
    inputDialog.setLabelText("اعداد بیشتر از ۱۰۰ به عنوان مبلغ و"
                             " کمتر از ۱۰۰ به عنوان درصد تخفیف محاسیه میشود");

    inputDialog.setInputMode(QInputDialog::IntInput);
    inputDialog.setIntRange(0, book.getPrice());

    inputDialog.setStyleSheet(
        "QInputDialog { background-color: #1A1512; border: 1px solid #C19A6B; border-radius: 12px; }"
        "QLabel { color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 13px;  }"
        "QLineEdit { background-color: #261F1A; border: 1px solid rgba(193, 154, 107, 0.4); border-radius: 6px; color: #FFF; padding: 6px; font-family: 'Segoe UI', 'Tahoma'; selection-background-color: #C19A6B; }"
        "QPushButton { background-color: rgba(193, 154, 107, 0.15); border: 1px solid rgba(193, 154, 107, 0.4); border-radius: 6px; color: #E6D7C3; font-family: 'Segoe UI', 'Tahoma'; font-size: 11px; min-width: 80px; padding: 6px; }"
        "QPushButton:hover { background-color: #C19A6B; color: #1E1914; font-weight: bold; }"
        );

    inputDialog.setOkButtonText("اعمال تخفیف");
    inputDialog.setCancelButtonText("انصراف");

    if (inputDialog.exec() == QDialog::Accepted) {
        int value = inputDialog.intValue();
        if(value > 100){
            emit discountRequested(book.getId(), (value/book.getPrice()) * 100.0);
        }
        else{
            emit discountRequested(book.getId(), value);
        }
    }
}


void SharedBookCard::on_active_checkBox_toggled(bool checked)
{
    if(isAdmin) return;

    old_active_state = !checked;

    ui->active_checkBox->setEnabled(false);

    QTimer::singleShot(10000, this, [this](){
        ui->active_checkBox->setEnabled(true);
    });

    emit changeActiveRequested(book.getId(), checked);
}


void SharedBookCard::handleCheckIsActive(const QString& action, const QJsonObject& data){
    if(action != "SET_BOOK_ACTIVE_STATE_RESPONSE"){
        return;
    }

    if(data.value("status").toString() != "SUCCESS"){
        ui->active_checkBox->setChecked(old_active_state);
    }
    else{
        old_active_state = ui->active_checkBox->isChecked();
    }
}

void SharedBookCard::on_study_pushButton_clicked()
{
    emit goToPDF(PDFpath);
}


void SharedBookCard::on_star_clicked()
{
    emit goToComments(book.getId());
}


void SharedBookCard::updatePrice(double discount){
    book.setPrice(book.getPrice(), discount);

    if(book.getDiscountPercentage() && book.getPrice()){
        ui->discount_label->show();
        ui->oldPrice_label->show();
        ui->discount_label->setText(QString::number(book.getDiscountPercentage(), 'f', 1) + "%");
        ui->oldPrice_label->setText(QString::number(book.getPrice(), 'f', 0));
        ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
        QFont font = ui->oldPrice_label->font();
        font.setStrikeOut(true);
        ui->oldPrice_label->setFont(font);
    }
    else{
        ui->discount_label->hide();
        ui->oldPrice_label->hide();
        ui->finalPrice_label->setText(QString::number(book.getFinalPrice(), 'f', 0));
    }
    if(book.getPrice() == 0){
        ui->finalPrice_label->setText("رایگان");
        ui->tooman_label->hide();
    }

}

int SharedBookCard::getBookID(){
    return book.getId();
}
