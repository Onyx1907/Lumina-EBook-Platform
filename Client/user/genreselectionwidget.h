#ifndef GENRESELECTIONWIDGET_H
#define GENRESELECTIONWIDGET_H

#include <QWidget>
#include <regularuser.h>

namespace Ui {
class GenreSelectionWidget;
}

class GenreSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GenreSelectionWidget(QString cur_username, QWidget *parent = nullptr);
    ~GenreSelectionWidget();

signals:
    void goToHomePage();

public slots:
    void onGeresChangeClicked();

private slots:
    void on_check_pushButton_clicked();

    void processNetworkData(const QString& action, const QJsonObject& data);

private:
    Ui::GenreSelectionWidget *ui;

    QString username;
};

#endif // GENRESELECTIONWIDGET_H
