#include "librarywidget.h"
#include "ui_librarywidget.h"
#include "QTabWidget"

LibraryWidget::LibraryWidget(int userID, QWidget *parent)
    : QWidget(parent), m_userID(userID)
    , ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

void LibraryWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    int currentIndex = ui->tabWidget->currentIndex();
    on_tabWidget_currentChanged(currentIndex);
}

void LibraryWidget::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        requestPurchasedBooks();
        break;
    case 1:
        requestSavedBooks();
        break;
    case 2:
        requestShelves();
        break;
    }
}


void LibraryWidget::requestPurchasedBooks(){

}

void LibraryWidget::requestSavedBooks(){

}

void LibraryWidget::requestShelves(){

}
