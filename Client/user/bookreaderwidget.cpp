#include "bookreaderwidget.h"
#include "ui_bookreaderwidget.h"
#include <QMessageBox>
#include <QPdfPageNavigator>
#include "clientnetworkmanager.h"
#include <QTimer>


BookReaderWidget::BookReaderWidget(int userId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookReaderWidget),
    m_userId(userId)
{
    ui->setupUi(this);

    m_document = new QPdfDocument(this);

    ui->widget->setDocument(m_document);
    ui->widget->setPageMode(QPdfView::PageMode::SinglePage);

    ui->error_label->hide();

    connect(&ClientNetworkManager::instance(), &ClientNetworkManager::responseReceived,
            this, &BookReaderWidget::processNetworkData);
}

BookReaderWidget::~BookReaderWidget()
{
    delete ui;
}

// ------------------ بخش شبکه و هماهنگی با API ------------------

void BookReaderWidget::loadBook(int bookID){
    m_bookId = bookID;

    m_document->close();

    m_currentPage = 0;
    ui->change_page_lineEdit->setText("1");
    ui->page_label->setText("/0");

    requestPdfPath();
}

void BookReaderWidget::requestPdfPath()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["book_id"] = m_bookId;
        data["user_id"] = m_userId;

        ClientNetworkManager::instance().sendRequest("GET_BOOK_PDF_PATH", data);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}

void BookReaderWidget::updateLastReadPage(int page){
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["book_id"] = m_bookId;
        data["user_id"] = m_userId;
        data["page"] = page;

        ClientNetworkManager::instance().sendRequest("UPDATE_LAST_READ_PAGE", data, true);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}


void BookReaderWidget::processNetworkData(const QString& action, const QJsonObject& data){
    if(action == "GET_BOOK_PDF_PATH_RESPONSE"){
        if(data["status"].toString() != "SUCCESS"){
            ui->error_label->setText(data["message"].toString());
            ui->error_label->show();
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
                ui->error_label->hide();
            });
        }
        else{
            // اگر مسیر با موفقیت آمد، فایل را لود می‌کنیم
            if (m_document->load(data["pdf_path"].toString()) == QPdfDocument::Error::None) {

                ui->page_label->setText(QString("/ %1").arg(m_document->pageCount()));

                // قدم دوم: حالا که فایل باز شد، آخرین صفحه خوانده شده را از سرور می‌خواهیم
                requestLastReadPage();
            }
            else{
                ui->error_label->setText("خطای ناشناخته");
                ui->error_label->show();
                QTimer::singleShot(3000, this, [this](){
                    ui->error_label->setText("");
                    ui->error_label->hide();
                });
            }
        }
    }
    else if(action == "GET_LAST_READ_PAGE_RESPONSE"){
        if(!data.contains("status")) {

            ui->error_label->setText("خطای ناشناخته");
            ui->error_label->show();
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
                ui->error_label->hide();
            });
            return;
        }

        if(data["status"].toString() == "SUCCESS"){
            handleLastReadPageResponse(data["page"].toInt());
        }
    }
    else if(action == "UPDATE_LAST_READ_PAGE_RESPONSE"){
        if(data["status"].toString() == "ERROR"){
            ui->error_label->setText(data["message"].toString());
            ui->error_label->show();
            QTimer::singleShot(3000, this, [this](){
                ui->error_label->setText("");
                ui->error_label->hide();
            });
        }
    }

}


void BookReaderWidget::requestLastReadPage()
{
    if(ClientNetworkManager::instance().connectToServer()){

        QJsonObject data;
        data["book_id"] = m_bookId;
        data["user_id"] = m_userId;

        ClientNetworkManager::instance().sendRequest("GET_LAST_READ_PAGE", data, true);
    }
    else{
        ui->error_label->setText("خطا در برقراری اتصال");
        ui->error_label->show();
        QTimer::singleShot(3000, this, [this](){
            ui->error_label->setText("");
            ui->error_label->hide();
        });
    }
}

// وقتی پاسخ سرور برای آخرین صفحه مطالعه شده آمد
void BookReaderWidget::handleLastReadPageResponse(int page)
{
    m_currentPage = page - 1;

    ui->widget->pageNavigator()->jump(m_currentPage, QPointF(), ui->widget->zoomFactor());

    // نوشتن شماره صفحه داخل لاین ادیت
    ui->change_page_lineEdit->setText(QString::number(page));
}


// ------------------ بخش کنترلرها و دکمه‌های UI ------------------

// دکمه صفحه بعد
void BookReaderWidget::on_next_pushButton_clicked()
{
    if (m_currentPage < m_document->pageCount() - 1) {
        m_currentPage++;

        ui->widget->pageNavigator()->jump(m_currentPage, QPointF(), ui->widget->zoomFactor());

        ui->change_page_lineEdit->setText(QString::number(m_currentPage + 1));
        updateLastReadPage(m_currentPage + 1);
    }
}


// دکمه صفحه قبل
void BookReaderWidget::on_prev_pushButton_clicked()
{
    if (m_currentPage > 0) {
        m_currentPage--;

        ui->widget->pageNavigator()->jump(m_currentPage, QPointF(), ui->widget->zoomFactor());

        ui->change_page_lineEdit->setText(QString::number(m_currentPage + 1));
        updateLastReadPage(m_currentPage + 1);
    }
}

// رفتن به صفحه مشخص وقتی کاربر در LineEdit عدد وارد کرده و اینتر می‌زند
void BookReaderWidget::on_change_page_lineEdit_returnPressed()
{
    bool ok;
    int targetPage = ui->change_page_lineEdit->text().toInt(&ok);

    if (ok && targetPage >= 1 && targetPage <= m_document->pageCount()){
        m_currentPage = targetPage - 1;

        ui->widget->pageNavigator()->jump(m_currentPage, QPointF(), ui->widget->zoomFactor());

        updateLastReadPage(targetPage);
    } else {
        ui->change_page_lineEdit->setText(QString::number(m_currentPage + 1));
    }
}

// بزرگ‌نمایی
void BookReaderWidget::on_zoomIn_pushButton_clicked()
{
    ui->widget->setZoomFactor(ui->widget->zoomFactor() + 0.2);
}

// کوچک‌نمایی
void BookReaderWidget::on_zoomOut_pushButton_clicked()
{
    if (ui->widget->zoomFactor() > 0.4) {
        ui->widget->setZoomFactor(ui->widget->zoomFactor() - 0.2);
    }
}

// دکمه بازگشت (بستن این صفحه)
void BookReaderWidget::on_back_pushButton_clicked()
{
    emit back();
    this->close();
}





