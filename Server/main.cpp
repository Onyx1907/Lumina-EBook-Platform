#include "mainwindow.h"
#include "server.h"

#include <QApplication>
#include <QMessageBox>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//----- راه اندازی سرور -----
    Server server;
    if (!server.start()) {
        QMessageBox::critical(nullptr, "Server Error"," .مشخص شده اجرا شود PORT , IP سرور نتوانست روی \n" " .را بررسی کنیدconstants.h لطفاً فایل ");
        return -1;
    }


//----- UI اجرای-----
    MainWindow w(&server);
    w.show();
    return a.exec();
}
