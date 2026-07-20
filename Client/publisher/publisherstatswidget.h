#ifndef PUBLISHERSTATSWIDGET_H
#define PUBLISHERSTATSWIDGET_H

#include <QWidget>

namespace Ui {
class PublisherStatsWidget;
}

class PublisherStatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherStatsWidget(QWidget *parent = nullptr);
    ~PublisherStatsWidget();

private:
    Ui::PublisherStatsWidget *ui;
};

#endif // PUBLISHERSTATSWIDGET_H
