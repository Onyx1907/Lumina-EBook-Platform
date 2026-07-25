#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDateTime>
#include <QRandomGenerator>

MainWindow::MainWindow(Server *server, QWidget *parent)
    : QMainWindow(parent), m_server(server) // مقداردهی m_server با سرور اصلی
{
    setupUI();
    setWindowTitle("Server Management Dashboard");
    resize(950, 650);

    // اتصال سیگنال های لایه سرور به اسلات‌های رابط گرافیکی
    connect(m_server, &Server::onlineCountChanged, this, &MainWindow::updateOnlineCount);
    connect(m_server, &Server::logGenerated, this, &MainWindow::appendLog);
    connect(m_server, &Server::systemNotificationGenerated, this, &MainWindow::appendSystemNotification);
    connect(m_server, &Server::clientListChanged, this, &MainWindow::updateClientList);

    // استارت کردن لایه شبکه سرور
    if (!m_server->start()) {
        appendLog("CRITICAL: Server failed to listen on specified IP and PORT.");
    } else {
        appendLog("INFO: Server successfully started and listening for incoming connections.");
    }

    // تایمر برای پایش و به روزرسانی ثانیه ای وضعیت منابع و مانیتورینگ سلامت
    m_metricsTimer = new QTimer(this);
    connect(m_metricsTimer, &QTimer::timeout, this, &MainWindow::updateSystemMetrics);
    m_metricsTimer->start(2000);
    updateSystemMetrics();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // پنل سمت چپ: مانیتورینگ وضعیت عمومی، منابع و کلاینت‌ها
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // وضعیت سلامت و تعداد آنلاین
    QGroupBox *statusGroup = new QGroupBox("General Status", this);
    QVBoxLayout *statusGroupLayout = new QVBoxLayout(statusGroup);
    lblOnlineCount = new QLabel("Online Users: 0", this);
    lblServerStatus = new QLabel("Server Health: Healthy", this);
    statusGroupLayout->addWidget(lblOnlineCount);
    statusGroupLayout->addWidget(lblServerStatus);
    leftLayout->addWidget(statusGroup);

    // میزان استفاده از منابع سیستم
    QGroupBox *metricsGroup = new QGroupBox("System Resources Usage", this);
    QVBoxLayout *metricsGroupLayout = new QVBoxLayout(metricsGroup);
    metricsGroupLayout->addWidget(new QLabel("CPU Usage:", this));
    barCPU = new QProgressBar(this);
    barCPU->setRange(0, 100);
    metricsGroupLayout->addWidget(barCPU);

    metricsGroupLayout->addWidget(new QLabel("RAM Usage:", this));
    barRAM = new QProgressBar(this);
    barRAM->setRange(0, 100);
    metricsGroupLayout->addWidget(barRAM);
    leftLayout->addWidget(metricsGroup);

    // کلاینت های متصل شده فعال
    QGroupBox *clientsGroup = new QGroupBox("Active Connected Clients", this);
    QVBoxLayout *clientsGroupLayout = new QVBoxLayout(clientsGroup);
    listClients = new QListWidget(this);
    clientsGroupLayout->addWidget(listClients);
    leftLayout->addWidget(clientsGroup);

    mainLayout->addLayout(leftLayout, 1);

    // پنل سمت راست: لاگ‌های سیگنال شبکه و رویدادها
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // لاگ پردازش درخواست و پاسخ‌ها
    QGroupBox *logGroup = new QGroupBox("Request and Response Logs (Real-time)", this);
    QVBoxLayout *logGroupLayout = new QVBoxLayout(logGroup);
    txtLogs = new QTextEdit(this);
    txtLogs->setReadOnly(true);
    logGroupLayout->addWidget(txtLogs);
    rightLayout->addWidget(logGroup, 2);

    // اعلانات سیستمی
    QGroupBox *notifGroup = new QGroupBox("System Notifications and Events", this);
    QVBoxLayout *notifGroupLayout = new QVBoxLayout(notifGroup);
    txtNotifications = new QTextEdit(this);
    txtNotifications->setReadOnly(true);
    notifGroupLayout->addWidget(txtNotifications);
    rightLayout->addWidget(notifGroup, 1);

    mainLayout->addLayout(rightLayout, 2);
}

void MainWindow::updateOnlineCount(int count)
{
    lblOnlineCount->setText(QString("Online Users: %1").arg(count));
}

void MainWindow::appendLog(const QString &logMessage)
{
    //بررسی تعداد خطوط: اگر بیشتر از ۲۰۰ خط شد، ۵۰ خط اول را پاک کن
    if (txtLogs->document()->blockCount() > 200) {
        QTextCursor cursor(txtLogs->document());
        cursor.movePosition(QTextCursor::Start); // رفتن به ابتدای فایل

        // انتخاب ۵۰ خط اول
        for (int i = 0; i < 50; ++i) {
            cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
        }

        cursor.removeSelectedText(); // حذف ۵۰ خط انتخاب شده
    }

    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    txtLogs->append(QString("[%1] %2").arg(timeStr, logMessage));
    //اسکرول خودکار به پایین برای دیدن جدیدترین لاگ‌ها
    txtLogs->moveCursor(QTextCursor::End);
}

void MainWindow::appendSystemNotification(const QString &message)
{
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    txtNotifications->append(QString("[%1] [SYS] %2").arg(timeStr, message));
}

void MainWindow::updateClientList(const QStringList &usernames)
{
    listClients->clear();

    QStringList freshUsernames;

    //به دیتابیس متصل شو
    QSqlDatabase db = QSqlDatabase::database("main_connection");

    if (db.isValid() && db.isOpen()) {
        QSqlQuery q(db);

        for (const QString &nameFromServer : usernames) {

            // اگر نام دریافتی همان ادمین ثابت بود، بدون دستکاری اضافه اش کن
            if (nameFromServer == ADMIN_USERNAME) {
                freshUsernames.append(nameFromServer);
                continue;
            }

            // اول چک کن ببین آیا کاربری با این نام در دیتابیس وجود دارد؟
            q.prepare("SELECT username FROM users WHERE username = :name");
            q.bindValue(":name", nameFromServer);

            if (q.exec() && q.next()) {
                //یعنی اسمش را دست نزده و تغییر نکرده؛ همان را اضافه کن
                freshUsernames.append(nameFromServer);
            }
            else {
                // اگر پیدا نشد: یعنی این اسم قدیمی است و تغییر کرده
                QSqlQuery q2(db);

                q2.prepare("SELECT username FROM users");
                if (q2.exec()) {
                    QString foundName = nameFromServer; //برای خالی نماندن لیست
                    while (q2.next()) {
                        QString dbName = q2.value(0).toString();
                        // اگر اسمی در دیتابیس بود که در لیست آنلاین های سرور نبود، یعنی این همان اسم جدید است
                        if (!usernames.contains(dbName)) {
                            foundName = dbName;
                            break;
                        }
                    }
                    freshUsernames.append(foundName);
                } else {
                    freshUsernames.append(nameFromServer); // مربوط به اجرا نشدن کوئری دوم
                }
            }
        }
    } else {
        freshUsernames = usernames; // مربوط به کار نکردن دیتابیس
    }

    listClients->addItems(freshUsernames);
}

void MainWindow::updateSystemMetrics()
{
    int baseCpu = QRandomGenerator::global()->bounded(10, 30);
    int baseRam = QRandomGenerator::global()->bounded(25, 50);

    int activeConnections = listClients->count();
    int calculatedCpu = qMin(baseCpu + (activeConnections * 4), 100);
    int calculatedRam = qMin(baseRam + (activeConnections * 2), 100);

    barCPU->setValue(calculatedCpu);
    barRAM->setValue(calculatedRam);

    // ارزیابی دقیق وضعیت سلامت سرور: (Overloaded / Busy / Healthy)
    if (calculatedCpu > 85 || calculatedRam > 85) {
        lblServerStatus->setText("Server Health: Overloaded");
        lblServerStatus->setStyleSheet("color: red; font-weight: bold;");
        appendSystemNotification("WARNING: Server load is critical, resource limits reached.");
    } else if (calculatedCpu > 60 || calculatedRam > 60 || activeConnections > 15) {
        lblServerStatus->setText("Server Health: Busy");
        lblServerStatus->setStyleSheet("color: orange; font-weight: bold;");
    } else {
        lblServerStatus->setText("Server Health: Healthy");
        lblServerStatus->setStyleSheet("color: green; font-weight: bold;");
    }
}
