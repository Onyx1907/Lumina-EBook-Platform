#ifndef EDITBOOKWIDGET_H
#define EDITBOOKWIDGET_H

#include <QWidget>

namespace Ui {
class EditBookWidget;
}

class EditBookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditBookWidget(QWidget *parent = nullptr);
    ~EditBookWidget();

private:
    Ui::EditBookWidget *ui;
};

#endif // EDITBOOKWIDGET_H
