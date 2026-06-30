#include "server.h"

// سازنده کلاس سرور: پایگاه داده را در بدو راه اندازی مقداردهی اولیه می کند
Server::Server(QObject* parent):QTcpServer(parent) {
    dbManager.initDatabase();
}
// راه اندازی سرور برای گوش دادن به آی پی و پورت مشخص شده در تنظیمات
bool Server::start(){
    if(!listen(QHostAddress(SERVER_IP),SERVER_PORT)){
        qDebug() << "Server listen failed:" << errorString();
        return false;
    }
    qDebug() << "Server listen on" << SERVER_IP << SERVER_PORT ;
    return true;
}
// مدیریت اتصال های ورودی: به محض اتصال هر کلاینت جدید، این متد اجرا میشود
void Server::incomingConnection(qintptr socketDescriptor){
   QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

   connect(socket,&QTcpSocket::readyRead,this,&Server::onReadyRead);
   connect(socket,&QTcpSocket::disconnected,this,&Server::onDisconnected);
}
// اسلات خواندن داده ها: زمان ارسال اطلاعات از طرف کلاینت فعال میشود
void Server::onReadyRead(){
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray raw = socket->readAll();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return;

    QJsonObject obj = doc.object();
    handleRequest(socket, obj);
}
// اسلات مدیریت قطع اتصال: حافظه سوکت کلاینتی که خارج شده را آزاد می کند
void Server::onDisconnected(){
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) socket->deleteLater();
}
// کلاینت را به متد مربوطه هدایت می کند "action" متد بررسی اولیه درخواست: بر اساس کلید
void Server::handleRequest(QTcpSocket* socket, const QJsonObject& obj){
    QString action = obj.value("action").toString();
    QJsonObject data = obj.value("data").toObject();

    if (action == "LOGIN") {
        handleLogin(socket, data);
    } else if (action == "REGISTER") {
        handleRegister(socket, data);
    } else if (action == "FORGOT_PASSWORD") {
        handleForgotPassword(socket, data);
    }
}
// برای تبدیل نوع شمارشی نقش کاربر به رشته متنی جهت ارسال در شبکه (Static) تابع کمکی محلی
static QString roleToString(UserRole role) {
    switch (role) {
    case UserRole::RegularUser: return " RegularUser";
    case UserRole::Publisher:   return "Publisher";
    case UserRole::Admin:       return "Admin";
    }
    return "User";
}
// مدیریت فرآیند ورود کاربران
void Server::handleLogin(QTcpSocket* socket, const QJsonObject& data){
    QString username = data.value("username").toString();
    QString passwordPlain = data.value("password").toString();

    UserRole role;
    bool isBlocked;
    int userId;
    int firstLogin;

    QJsonObject resp;
    resp["action"] = "LOGIN_RESPONSE";

    if (!dbManager.verifyUser(username, passwordPlain, role, isBlocked, userId, firstLogin)) {
        resp["status"] = "FAILED";
        resp["message"] = ".نام کاربری یا رمز عبور اشتباه است یا حساب شما مسدود است";
        sendJson(socket, resp);
        return;
    }

    resp["status"] = "SUCCESS";
    resp["message"] = "!خوش آمدی";
    resp["user_role"] = roleToString(role);
    resp["first_login"] = firstLogin;

    sendJson(socket, resp);

    if (firstLogin == 1) {
        dbManager.setFirstLoginFalse(userId);
    }
}
// مدیریت فرآیند ثبت نام کاربر جدید
void Server::handleRegister(QTcpSocket* socket, const QJsonObject& data){
        QString username = data.value("username").toString();
        QString passwordPlain = data.value("password").toString();
        QString roleStr = data.value("role").toString();
        QString securityQuestion = data.value("security_question").toString();
        QString securityAnswerPlain = data.value("security_answer").toString();

        UserRole role = UserRole::RegularUser;
        if (roleStr == "Publisher") role = UserRole::Publisher;
        else if (roleStr == "Admin") role = UserRole::Admin;

        QJsonObject resp;
        resp["action"] = "REGISTER_RESPONSE";

        if (dbManager.isUsernameTaken(username)) {
            resp["status"] = "FAILED";
            resp["message"] = ".نام کاربری تکراری است";
            sendJson(socket, resp);
            return;
        }
        if (!dbManager.registerUser(username, passwordPlain, role, securityQuestion, securityAnswerPlain)) {
            resp["status"] = "FAILED";
            resp["message"] = ".خطا در ثبت نام";
            sendJson(socket, resp);
            return;
        }

        resp["status"] = "SUCCESS";
        resp["message"] = ".ثبت نام با موفقیت انجام شد";
        sendJson(socket, resp);
}
// مدیریت دو مرحله ای فرآیند فراموشی رمز عبور
void Server::handleForgotPassword(QTcpSocket* socket, const QJsonObject& data){
    QString step = data.value("step").toString();

    QJsonObject resp;

    if (step == "REQUEST_QUESTION") {
        QString username = data.value("username").toString();
        QString question;
        resp["action"] = "FORGOT_PASSWORD_RESPONSE";

        if (!dbManager.getSecurityQuestion(username, question)) {
            resp["status"] = "FAILED";
            resp["message"] = ".کاربر یافت نشد";
        } else {
            resp["status"] = "SUCCESS";
            resp["security_question"] = question;
        }
         sendJson(socket, resp);
    }
    else if (step == "ANSWER_AND_RESET") {
        QString username = data.value("username").toString();
        QString answerPlain = data.value("security_answer").toString();
        QString newPasswordPlain = data.value("new_password").toString();

        resp["action"] = "FORGOT_PASSWORD_RESPONSE";

        if (!dbManager.verifySecurityAnswerAndResetPassword(username,answerPlain,newPasswordPlain)) {
            resp["status"] = "FAILED";
            resp["message"] = ".پاسخ امنیتی نادرست است";
        }
        else {
            resp["status"] = "SUCCESS";
            resp["message"] = ".رمز عبور با موفقیت تغییر کرد";
        }
        sendJson(socket, resp);
    }
}
//  :متد ارسال اطلاعات
//را فشرده کرده و از طریق سوکت شبکه به کلاینت می فرستد JSON شی
void Server::sendJson(QTcpSocket* socket, const QJsonObject& obj) {
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact);
    socket->write(bytes);
    socket->flush();
}

