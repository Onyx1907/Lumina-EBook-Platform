#include "server.h"
#include <utility>


//***************************************************احراز هویت مرکزی******************************************************


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
    if (socket){
        QString username = socketToUser.value(socket);
        onlineUsers.remove(username);
        socketToUser.remove(socket);
        socket->deleteLater();
    }
}
// کلاینت را به متد مربوطه هدایت می کند "action" متد بررسی اولیه درخواست: بر اساس کلید
void Server::handleRequest(QTcpSocket* socket, const QJsonObject& obj){
    QString action = obj.value("action").toString();
    QJsonObject data = obj.value("data").toObject();

    //***************************************************احراز هویت مرکزی******************************************************

    if (action == "LOGIN") {
        handleLogin(socket, data);return;
    } if (action == "REGISTER") {
        handleRegister(socket, data);return;
    } if (action == "FORGOT_PASSWORD") {
        handleForgotPassword(socket, data);return;
    }

    //*********************************************پنل کاربر عادی ( ماژول 1 ) *************************************************
    if (action == "SET_FAVORITE_GENRES") {
        handleSetFavoriteGenres(socket, data);
        return;
    }
    if (action == "GET_RECOMMENDED_BOOKS") {
        handleGetRecommendedBooks(socket, data);
        return;
    }
    if (action == "GET_BOOKS_BY_GENRE") {
        handleGetBooksByGenre(socket, data);
        return;
    }

    if (action == "GET_POPULAR_BOOKS") {
        handleGetPopularBooks(socket);
        return;
    }
    if (action == "GET_NEW_BOOKS") {
        handleGetNewBooks(socket);
        return;
    }
    if (action == "GET_BESTSELLERS") {
        handleGetBestsellers(socket);
        return;
    }
    if (action == "GET_FREE_BOOKS") {
        handleGetFreeBooks(socket);
        return;
    }

    if (action == "GET_PROFILE") {
        handleGetProfile(socket, data);
        return;
    }
    if (action == "UPDATE_PROFILE") {
        handleUpdateProfile(socket, data);
        return;
    }
    if (action == "CHANGE_PASSWORD") {
        handleChangePassword(socket, data);
        return;
    }

    if (action == "GET_PURCHASE_HISTORY") {
        handleGetPurchaseHistory(socket, data);
        return;
    }

    if (action == "CHECK_BOOK_OWNERSHIP") {
        handleCheckBookOwnership(socket, data);
        return;
    }

    if (action == "GET_BOOK_PDF_PATH") {
        handleGetBookPdfPath(socket, data);
        return;
    }

    if (action == "STREAM_FILE_DATA") {
        handleStreamFileData(socket, data);
        return;
    }

    QJsonObject resp;
    resp["action"] = action + "_RESPONSE";
    resp["status"] = "ERROR";
    resp["message"] = ".درخواست نامعتبر است";
    sendJson(socket, resp);


    //*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************


    if (action == "SEARCH_BOOKS") {
        handleSearchBooks(socket, data);
        return;
    }


    //*********************************************پنل کاربر عادی ( ماژول 3 )****************************************************


    if (action == "ADD_COMMENT") { handleAddComment(socket, data); return; }
    if (action == "EDIT_COMMENT") { handleEditComment(socket, data); return; }
    if (action == "DELETE_COMMENT") { handleDeleteComment(socket, data); return; }
    if (action == "GET_COMMENTS") { handleGetComments(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 4 )****************************************************


    if (action == "ADD_TO_CART") return handleAddToCart(socket, data);
    if (action == "REMOVE_FROM_CART") return handleRemoveFromCart(socket, data);
    if (action == "GET_CART") return handleGetCart(socket, data);
    if (action == "FINALIZE_PURCHASE") return handleFinalizePurchase(socket, data);


}

//***************************************************احراز هویت مرکزی******************************************************


// برای تبدیل نوع شمارشی نقش کاربر به رشته متنی جهت ارسال در شبکه (Static) تابع کمکی محلی
static QString roleToString(UserRole role) {
    switch (role) {
    case UserRole::RegularUser: return "RegularUser";
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
    resp["action"] = "FORGOT_PASSWORD_RESPONSE"; // مقدار اکشن را در همان ابتدا ثابت می‌گذاریم تا در هر دو حالت ارسال شود

    if (step == "REQUEST_QUESTION") {
        QString username = data.value("username").toString();
        QString question;

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

        if (!dbManager.verifySecurityAnswerAndResetPassword(username, answerPlain, newPasswordPlain)) {
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


//*********************************************پنل کاربر عادی ( ماژول 1 ) *************************************************


// ذخیره ژانرهای مورد علاقه کاربر در پایگاه داده
void Server::handleSetFavoriteGenres(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    QJsonArray arr = data.value("genres").toArray();

    if (arr.size() < 1 || arr.size() > 3) {
        QJsonObject resp;
        resp["action"] = "SET_FAVORITE_GENRES_RESPONSE";
        resp["status"] = "ERROR";
        resp["message"] = ".تعداد ژانر باید بین ۱ تا ۳ باشد";
        sendJson(socket, resp);
        return;
    }

    QStringList genres;
    for(const QJsonValue& v : std::as_const(arr))
        genres.append(v.toString());

    bool ok = dbManager.setFavoriteGenres(username, genres);

    if (ok) {
        dbManager.setFirstLoginFalseByUsername(username);
    }

    QJsonObject resp;
    resp["action"] = "SET_FAVORITE_GENRES_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".ژانرهای مورد علاقه با موفقیت ذخیره شدند"
                         : ".خطا در ذخیره ژانرهای مورد علاقه";

    sendJson(socket, resp);
}

// دریافت لیست کتاب های پیشنهادی بر اساس ژانرهای مورد علاقه کاربر
void Server::handleGetRecommendedBooks(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    QStringList genres = dbManager.getFavoriteGenres(username);
    QList<QJsonObject> books = dbManager.getRecommendedBooks(genres);

    QJsonArray arr;
    for (QJsonObject b : std::as_const(books)) {
        // خواندن مسیر عکس از دیتابیس
        QString coverPath = b.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = b.value("coverImagePath").toString();

        QFile coverFile(coverPath);
        if (!coverPath.isEmpty() && coverFile.exists() && coverFile.open(QIODevice::ReadOnly)) {
            QByteArray coverBytes = coverFile.readAll();
            coverFile.close();
            b["cover_base64"] = QString::fromLatin1(coverBytes.toBase64());
        } else {
            b["cover_base64"] = "";
        }


        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_RECOMMENDED_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

//دریافت لیست کتاب ها بر اساس یک ژانر خاص
void Server::handleGetBooksByGenre(QTcpSocket* socket, const QJsonObject& data) {
    const QString genre = data.value("genre").toString();
    QList<QJsonObject> books = dbManager.getBooksByGenre(genre);

    QJsonArray arr;
    for (QJsonObject b : std::as_const(books)) {
        QString coverPath = b.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = b.value("coverImagePath").toString();

        QFile coverFile(coverPath);
        if (!coverPath.isEmpty() && coverFile.exists() && coverFile.open(QIODevice::ReadOnly)) {
            QByteArray coverBytes = coverFile.readAll();
            coverFile.close();
            b["cover_base64"] = QString::fromLatin1(coverBytes.toBase64());
        } else {
            b["cover_base64"] = "";
        }


        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_BOOKS_BY_GENRE_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

//دریافت لیست کتاب های محبوب (پرطرفدار)
void Server::handleGetPopularBooks(QTcpSocket* socket) {
    QList<QJsonObject> books = dbManager.getPopularBooks();
    QJsonArray arr;
    for (QJsonObject b : std::as_const(books)) {
        QString coverPath = b.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = b.value("coverImagePath").toString();

        QFile coverFile(coverPath);
        if (!coverPath.isEmpty() && coverFile.exists() && coverFile.open(QIODevice::ReadOnly)) {
            QByteArray coverBytes = coverFile.readAll();
            coverFile.close();
            b["cover_base64"] = QString::fromLatin1(coverBytes.toBase64());
        } else {
            b["cover_base64"] = "";
        }


        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_POPULAR_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

//دریافت لیست جدیدترین کتاب های اضافه شده
void Server::handleGetNewBooks(QTcpSocket* socket) {
    QList<QJsonObject> books = dbManager.getNewBooks();
    QJsonArray arr;
    for (QJsonObject b : std::as_const(books)) {
        QString coverPath = b.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = b.value("coverImagePath").toString();

        QFile coverFile(coverPath);
        if (!coverPath.isEmpty() && coverFile.exists() && coverFile.open(QIODevice::ReadOnly)) {
            QByteArray coverBytes = coverFile.readAll();
            coverFile.close();
            b["cover_base64"] = QString::fromLatin1(coverBytes.toBase64());
        } else {
            b["cover_base64"] = "";
        }


        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_NEW_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

//دریافت لیست کتاب های پرفروش
void Server::handleGetBestsellers(QTcpSocket* socket) {
    QList<QJsonObject> books = dbManager.getBestsellers();
    QJsonArray arr;
    for (QJsonObject b : std::as_const(books)) {
        QString coverPath = b.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = b.value("coverImagePath").toString();

        QFile coverFile(coverPath);
        if (!coverPath.isEmpty() && coverFile.exists() && coverFile.open(QIODevice::ReadOnly)) {
            QByteArray coverBytes = coverFile.readAll();
            coverFile.close();
            b["cover_base64"] = QString::fromLatin1(coverBytes.toBase64());
        } else {
            b["cover_base64"] = "";
        }


        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_BESTSELLERS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

//دریافت لیست کتاب های رایگان
void Server::handleGetFreeBooks(QTcpSocket* socket) {
    QList<QJsonObject> books = dbManager.getFreeBooks();
    QJsonArray arr;
    for (QJsonObject b : std::as_const(books)) {
        QString coverPath = b.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = b.value("coverImagePath").toString();

        QFile coverFile(coverPath);
        if (!coverPath.isEmpty() && coverFile.exists() && coverFile.open(QIODevice::ReadOnly)) {
            QByteArray coverBytes = coverFile.readAll();
            coverFile.close();
            b["cover_base64"] = QString::fromLatin1(coverBytes.toBase64());
        } else {
            b["cover_base64"] = "";
        }


        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_FREE_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}
// دریافت اطلاعات حساب کاربری (پروفایل)
void Server::handleGetProfile(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    QJsonObject profile = dbManager.getUserProfile(username);

    QJsonObject resp;
    resp["action"] = "GET_PROFILE_RESPONSE";
    if (profile.isEmpty()) {
        resp["status"] = "ERROR";
        resp["message"] = ".کاربر یافت نشد";
    } else {
        resp["status"] = "SUCCESS";
        resp["profile"] = profile;
    }
    sendJson(socket, resp);
}
// به روزرسانی اطلاعات پروفایل کاربر
void Server::handleUpdateProfile(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    const QString name = data.value("name").toString();
    const QString email = data.value("email").toString();

    bool ok = dbManager.updateUserProfile(username, name, email);

    QJsonObject resp;
    resp["action"] = "UPDATE_PROFILE_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".اطلاعات حساب کاربری با موفقیت به روزرسانی شد"
                         : ".خطا در به روزرسانی اطلاعات حساب کاربری";

    sendJson(socket, resp);
}
// تغییر رمز عبور کاربر
void Server::handleChangePassword(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    const QString oldPass = data.value("old_password").toString();
    const QString newPass = data.value("new_password").toString();

    bool ok = dbManager.changePassword(username, oldPass, newPass);

    QJsonObject resp;
    resp["action"] = "CHANGE_PASSWORD_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".رمز عبور با موفقیت تغییر کرد "
                         : ".رمز عبور فعلی اشتباه است یا کاربر یافت نشد";

    sendJson(socket, resp);
}
// دریافت تاریخچه خریدهای یک کاربر
void Server::handleGetPurchaseHistory(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    QList<QJsonObject> history = dbManager.getPurchaseHistory(username);

    QJsonArray arr;
    for (const QJsonObject& h : std::as_const(history))
        arr.append(h);

    QJsonObject resp;
    resp["action"] = "GET_PURCHASE_HISTORY_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["history"] = arr;
    sendJson(socket, resp);
}

// متد بررسی وضعیت خرید و اطلاعات تکمیلی کتاب در لحظه کلیک
void Server::handleCheckBookOwnership(QTcpSocket* socket, const QJsonObject& data)
{
    int userId = data.value("user_id").toInt();
    int bookId = data.value("book_id").toInt();

    QString publisher = "";
    double rating = 0.0;
    QString coverPath = "";


    QJsonObject resp;
    resp["action"] = "CHECK_BOOK_OWNERSHIP_RESPONSE";
    resp["book_id"] = bookId;

    if (!dbManager.getActiveBookDetails(bookId, publisher, rating, coverPath)) {
        resp["status"] = "FAILED";
        resp["message"] = ".این کتاب در حال حاضر غیرفعال یا ناموجود است";
        sendJson(socket, resp);
        return;
    }

    // بررسی وضعیت خرید از جدول
    bool purchased = dbManager.isBookPurchased(userId, bookId);

    resp["status"] = "SUCCESS";
    resp["is_purchased"] = purchased;
    resp["publisher_name"] = publisher;
    resp["rating"] = rating;

    sendJson(socket, resp);
}

// متد بررسی و فرستادن آدرس فایل پی‌دی‌اف برای دانلود
void Server::handleGetBookPdfPath(QTcpSocket* socket, const QJsonObject& data)
{
    int userId = data.value("user_id").toInt();
    int bookId = data.value("book_id").toInt();

    QJsonObject resp;
    resp["action"] = "GET_BOOK_PDF_PATH_RESPONSE";
    resp["book_id"] = bookId;

    if (!dbManager.isBookPurchased(userId, bookId)) {
        resp["status"] = "FAILED";
        resp["message"] = ".شما دسترسی به این کتاب ندارید. ابتدا باید آن را خریداری کنید";
        sendJson(socket, resp);
        return;
    }

    // گرفتن آدرس فیزیکی فایل از جدول
    QString pdfPath = dbManager.getBookPdfPath(bookId);

    if (!pdfPath.isEmpty()) {
        resp["status"] = "SUCCESS";
        resp["pdf_path"] = pdfPath;
    } else {
        resp["status"] = "FAILED";
        resp["message"] = ".فایل پی‌دی‌اف این کتاب یافت نشد";
    }

    sendJson(socket, resp);
}

void Server::handleStreamFileData(QTcpSocket* socket, const QJsonObject& data)
{
    QString filePath = data.value("file_path").toString(); // آدرس فیزیکی فایل روی سرور

    QFile file(filePath);
    // اگر فایل باز نشد، خطای جی‌سون می‌فرستیم
    if (!file.open(QIODevice::ReadOnly)) {
        QJsonObject resp;
        resp["action"] = "STREAM_FILE_DATA_RESPONSE";
        resp["status"] = "FAILED";
        resp["message"] = ".فایل مورد نظر روی سرور یافت نشد";
        sendJson(socket, resp);
        return;
    }

    const qint64 chunkSize = 4096; // هر بار فقط ۴ کیلوبایت ارسال می‌شود
    while (!file.atEnd()) {
        QByteArray chunk = file.read(chunkSize);
        socket->write(chunk);
    }

    file.close();
    socket->flush(); // تخلیه نهایی سوکت و پایان ارسال بایت‌ها
    return;
}


//*********************************************پنل کاربر عادی ( ماژول 2 )****************************************************


// مدیریت درخواست جستجوی کتاب
void Server::handleSearchBooks(QTcpSocket* socket, const QJsonObject& data)
{
    QString title = data.value("title").toString();
    QString author = data.value("author").toString();
    QString publisher = data.value("publisher_name").toString();

    //دریافت نتایج جستجو از دیتابیس (که مسیرهای فیزیکی فایل ها را هم شامل میشود)
    QList<QJsonObject> books = dbManager.searchBooks(title, author, publisher);
    QJsonArray finalArray;

    for (QJsonObject &book : books) {
    //استخراج مسیر فیزیکی عکس کاور کتاب
        QString coverPath = book.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = book.value("coverImagePath").toString();
        }

        QFile coverFile(coverPath);
        if (!coverPath.isEmpty() && coverFile.exists() && coverFile.open(QIODevice::ReadOnly)) {
            QByteArray coverBytes = coverFile.readAll();
            coverFile.close();

            book["cover_base64"] = QString::fromUtf8(coverBytes.toBase64());
        } else {
            book["cover_base64"] = ""; // در صورت عدم وجود تصویر
        }


        finalArray.append(book);
    }

    QJsonObject resp;
    resp["action"] = "SEARCH_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = finalArray;

    sendJson(socket, resp);
}


//*********************************************پنل کاربر عادی ( ماژول 3 )***************************************************

//مدیریت ثبت نظرات
void Server::handleAddComment(QTcpSocket* socket, const QJsonObject& data) {
    int bookId = data["book_id"].toInt();
    int userId = data["user_id"].toInt();
    QString text = data["text"].toString();
    int rating = data["rating"].toInt();

    bool ok = dbManager.addComment(bookId, userId, text, rating);

    QJsonObject resp;
    resp["action"] = "ADD_COMMENT_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".نظر ثبت شد"
                         :".خطا در ثبت نظر";

    sendJson(socket, resp);

    if (ok) {
        QJsonObject b;
        b["action"] = "COMMENT_UPDATED";
        b["book_id"] = bookId;
        b["type"] = "ADD";
        broadcastToAll(b);
    }
}

//مدیریت ویرایش نظرات
void Server::handleEditComment(QTcpSocket* socket, const QJsonObject& data) {
    int commentId = data["comment_id"].toInt();
    QString text = data["text"].toString();
    int rating = data["rating"].toInt();

    bool ok = dbManager.editComment(commentId, text, rating);

    QJsonObject resp;
    resp["action"] = "EDIT_COMMENT_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".نظر ویرایش شد"
                         : ".خطا در ویرایش";

    sendJson(socket, resp);

    if (ok) {
        QJsonObject b;
        b["action"] = "COMMENT_UPDATED";
        b["comment_id"] = commentId;
        b["type"] = "EDIT";
        broadcastToAll(b);
    }
}

//مدیریت حذف نظرات
void Server::handleDeleteComment(QTcpSocket* socket, const QJsonObject& data) {
    int commentId = data["comment_id"].toInt();

    bool ok = dbManager.deleteComment(commentId);

    QJsonObject resp;
    resp["action"] = "DELETE_COMMENT_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".نظر حذف شد"
                         : ".خطا در حذف";

    sendJson(socket, resp);

    if (ok) {
        QJsonObject b;
        b["action"] = "COMMENT_UPDATED";
        b["comment_id"] = commentId;
        b["type"] = "DELETE";
        broadcastToAll(b);
    }
}

//مدیریت دیدن نظرات
void Server::handleGetComments(QTcpSocket* socket, const QJsonObject& data) {
    int bookId = data["book_id"].toInt();

    QList<QJsonObject> list = dbManager.getCommentsForBook(bookId);

    QJsonArray arr;
    for (auto &c : list)
        arr.append(c);

    QJsonObject resp;
    resp["action"] = "GET_COMMENTS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["comments"] = arr;
    sendJson(socket, resp);
}

//ارسال پیام به همه کلاینت ها
void Server::broadcastToAll(const QJsonObject& obj) {
    for (QTcpSocket* client : std::as_const(clients)) {
        sendJson(client, obj);
    }
}


//*********************************************پنل کاربر عادی ( ماژول 4 )****************************************************


//مدیریت افزودن به سبد خرید
void Server::handleAddToCart(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = dbManager.addToCart(userId, bookId);

    QJsonObject resp;
    resp["action"] = "ADD_TO_CART_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".کتاب به سبد خرید اضافه شد"
                         : ".خطا در افزودن کتاب";

    sendJson(socket, resp);
}

//مدیریت حذف از سبد خرید
void Server::handleRemoveFromCart(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = dbManager.removeFromCart(userId, bookId);

    QJsonObject resp;
    resp["action"] = "REMOVE_FROM_CART_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".کتاب از سبد خرید حذف شد"
                         : ".خطا در حذف کتاب";

    sendJson(socket, resp);
}

//مدیریت گرفتن یک لیست از سبد خرید
void Server::handleGetCart(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    QList<QJsonObject> items = dbManager.getCartItems(userId);

    QJsonArray arr;
    double total = 0;
    double discountTotal = 0;

    for (auto &i : items) {
        double price = i["price"].toDouble();
        double discount = i["discount"].toDouble();

        discountTotal += price * (discount / 100.0);
        total += price;

        arr.append(i);
    }

    double finalPrice = total - discountTotal;


    QJsonObject resp;
    resp["action"] = "GET_CART_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["items"] = arr;
    resp["total_price"] = total;
    resp["discount"] = discountTotal;
    resp["final_price"] = finalPrice;
    resp["count"] = items.count();

    sendJson(socket, resp);
}

//مدیریت نهایی کردن سبد خرید
void Server::handleFinalizePurchase(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    bool ok = dbManager.finalizePurchase(userId);

    QJsonObject resp;
    resp["action"] = "FINALIZE_PURCHASE_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".خرید با موفقیت انجام شد"
                         : ".خطا در نهایی سازی خرید";

    sendJson(socket, resp);
}