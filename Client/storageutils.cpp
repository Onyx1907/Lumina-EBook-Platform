#include "storageutils.h"
#include <QPixmap>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

void StorageUtils::displayBookCover(const QString& absolutePathFromServer, QLabel* labelTarget)
{
    if (absolutePathFromServer.isEmpty() || !labelTarget) {
        return;
    }

    qDebug() << "Rendering image from:" << absolutePathFromServer;

    QPixmap bookImage(absolutePathFromServer);
    if (!bookImage.isNull()) {
        QPixmap scaledImage = bookImage.scaled(labelTarget->size(),
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);
        labelTarget->setPixmap(scaledImage);
        labelTarget->setAlignment(Qt::AlignCenter);
    }
}

void StorageUtils::openBookPdf(const QString& absolutePdfPathFromServer)
{
    if (absolutePdfPathFromServer.isEmpty()) {
        qDebug() << "PDF path is empty!";
        return;
    }

    QUrl pdfUrl = QUrl::fromLocalFile(absolutePdfPathFromServer);
    bool success = QDesktopServices::openUrl(pdfUrl);

    if (!success) {
        qDebug() << "Could not open PDF file. Check default system reader.";
    }
}
