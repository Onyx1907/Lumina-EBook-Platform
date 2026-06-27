#include "registerwidget.h"
#include "ui_registerwidget.h"
#include <QMessageBox>
#include <QPushButton>

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::on_back_pushButton_clicked()
{
    QWidget *parent = this->parentWidget();
    if(parent){
        QStackedWidget *stacked = qobject_cast<QStackedWidget*>(parent);
        if(stacked){
            stacked->setCurrentIndex(MainWindow::Page::LoginPageIndex);
        }
    }
}

