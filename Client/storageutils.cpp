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

    QPixmap bookImage(absolutePathFromServer);
    if (!bookImage.isNull()) {
        QPixmap scaledImage = bookImage.scaled(labelTarget->size(),
                                               Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation);
        labelTarget->setPixmap(scaledImage);
        labelTarget->setAlignment(Qt::AlignCenter);
    }
}
