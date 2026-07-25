#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "server.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Server *server, QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void updateOnlineCount(int count);
    void appendLog(const QString &logMessage);
    void appendSystemNotification(const QString &message);
    void updateClientList(const QStringList &usernames);

    // تایمر داخلی برای محاسبه زمان واقعی منابع سیستم و سلامت سرور
    void updateSystemMetrics();

private:
    void setupUI();

    Server *m_server;
    QTimer *m_metricsTimer;

    // المان  های گرافیکی داشبورد مدیریتی
    QLabel *lblOnlineCount;
    QLabel *lblServerStatus;
    QProgressBar *barCPU;
    QProgressBar *barRAM;
    QTextEdit *txtLogs;
    QTextEdit *txtNotifications;
    QListWidget *listClients;
};


#endif // MAINWINDOW_H