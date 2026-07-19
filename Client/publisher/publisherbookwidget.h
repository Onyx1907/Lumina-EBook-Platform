#ifndef PUBLISHERBOOKWIDGET_H
#define PUBLISHERBOOKWIDGET_H

#include <QWidget>

namespace Ui {
class PublisherBookWidget;
}

class PublisherBookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherBookWidget(QWidget *parent = nullptr);
    ~PublisherBookWidget();

private:
    Ui::PublisherBookWidget *ui;
};

#endif // PUBLISHERBOOKWIDGET_H
