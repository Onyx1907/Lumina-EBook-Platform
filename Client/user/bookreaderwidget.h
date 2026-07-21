#ifndef BOOKREADERWIDGET_H
#define BOOKREADERWIDGET_H

#include <QWidget>
#include <QPdfDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class BookReaderWidget; }
QT_END_NAMESPACE

class BookReaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookReaderWidget(int userId, QWidget *parent = nullptr);
    ~BookReaderWidget();

    void loadBook(int bookID);
    void loadBook(QString path);

    void handleLastReadPageResponse(int page);

signals:
    void back();

private slots:

    void processNetworkData(const QString& action, const QJsonObject& data);

    void on_back_pushButton_clicked();

    void on_next_pushButton_clicked();

    void on_prev_pushButton_clicked();

    void on_change_page_lineEdit_returnPressed();

    void on_zoomIn_pushButton_clicked();

    void on_zoomOut_pushButton_clicked();

private:
    Ui::BookReaderWidget *ui;
    QPdfDocument *m_document;

    int m_currentPage = 0;

    int m_userId;
    int m_bookId;

    void requestPdfPath();       // ارسال درخواست GET_BOOK_PDF_PATH
    void requestLastReadPage();  // ارسال درخواست GET_LAST_READ_PAGE
    void updateLastReadPage(int page);
};

#endif // BOOKREADERWIDGET_H
