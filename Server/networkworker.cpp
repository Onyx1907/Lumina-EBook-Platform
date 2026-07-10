#include "networkWorker.h"

void NetworkWorker::sendJsonToClient(const QJsonObject& obj) {
    if (m_socket && m_socket->isOpen()) {
        sendJson(m_socket, obj); // از تابع اصلی خودت استفاده میکند
    }
}

//تابع کمکی استاتیک
static QString roleToString(UserRole role) {
    switch (role) {
    case UserRole::RegularUser: return "RegularUser";
    case UserRole::Publisher:   return "Publisher";
    case UserRole::Admin:       return "Admin";
    }
    return "RegularUser";
}

// در سازنده networkWorker.cpp دیتابیس سرور را نگیر، جایش این کار را بکن:
NetworkWorker::NetworkWorker(qintptr socketDescriptor, DatabaseManager* dbManager, QObject* parent)
    : QObject(parent), m_socketDescriptor(socketDescriptor)
{

    m_dbManager = *dbManager;

    m_socket = nullptr;
}

// در متدی که با شروع ترد صدا زده می‌شود (مثلاً startProcessing)
void NetworkWorker::startProcessing() {
    // ۱. ساخت سوکت در داخل خودِ ترد فرعی (خیلی مهم)
    m_socket = new QTcpSocket(this);
    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        emit finished();
        return;
    }
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkWorker::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkWorker::finished);

    // ۲. حل مشکل دیتابیس: ساخت یک کانکشن کاملاً اختصاصی برای همین ترد
    // استفاده از آیدی ترد به عنوان نام کانکشن باعث می‌شود نام‌ها کاملاً منحصربه‌فرد باشند
    QString connectionName = "Thread_Db_" + QString::number(quintptr(QThread::currentThreadId()));

    // باز کردن یک کانکشن جدید به دیتابیس (مثلاً SQLite)
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("database_name.db"); // نام فایل دیتابیس خودت را اینجا بگذار

    if (!db.open()) {
        qCritical() << "Worker Thread could not open database:" << connectionName;
    }
}

void NetworkWorker::onReadyRead() {
    // استفاده از حلقه در صورتی که چندین پیام با هم رسیده باشند (TCP Stream Buffer)
    while (m_socket->canReadLine()) {
        QByteArray line = m_socket->readLine().trimmed();
        if (line.isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);

        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "Worker JSON Parse Error:" << err.errorString();
            continue; // رد کردن این خط خراب و ادامه دادن برای خط‌های بعدی
        }

        QJsonObject obj = doc.object();
        handleRequest(m_socket, obj);
    }
}

void NetworkWorker::onDisconnected() {
    //ابتدا به سرور خبر می‌دهیم که این سوکت دیسکانکت شد تا از مپ آنلاین‌ها حذف شود
    emit userDisconnected(m_socket);

    //به خودِ سوکت دستور میدهیم که به محض پایان کارهای جاری، حافظه خودش را آزاد کند
    if (m_socket) {
        m_socket->deleteLater();
    }

    //ترد فرعی را خاتمه میدهیم
    emit finished();
}

void NetworkWorker::sendJson(QTcpSocket* socket, const QJsonObject& obj) {
    if (!socket || !socket->isOpen()) return;

    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson(QJsonDocument::Compact);

    // اضافه کردن کاراکتر \n به انتهای پیام تا کلاینت هم بتواند آن را خط‌به‌خط بخواند
    bytes.append('\n');

    socket->write(bytes);
    socket->flush();
}

void NetworkWorker::handleRequest(QTcpSocket* socket, const QJsonObject& obj) {
    QString action = obj.value("action").toString();
    QJsonObject data = obj.value("data").toObject();


    //***************************************************احراز هویت مرکزی******************************************************

    if (action == "LOGIN") { handleLogin(socket, data); return; }
    else if (action == "REGISTER") { handleRegister(socket, data); return; }
    else if (action == "FORGOT_PASSWORD") { handleForgotPassword(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 1 ) *************************************************

    else if (action == "SET_FAVORITE_GENRES") { handleSetFavoriteGenres(socket, data); return; }
    else if (action == "GET_RECOMMENDED_BOOKS") { handleGetRecommendedBooks(socket, data); return; }
    else if (action == "GET_BOOKS_BY_GENRE") { handleGetBooksByGenre(socket, data); return; }
    else if (action == "GET_POPULAR_BOOKS") { handleGetPopularBooks(socket); return; }
    else if (action == "GET_NEW_BOOKS") { handleGetNewBooks(socket); return; }
    else if (action == "GET_BESTSELLERS") { handleGetBestsellers(socket); return; }
    else if (action == "GET_FREE_BOOKS") { handleGetFreeBooks(socket); return; }
    else if (action == "GET_PROFILE") { handleGetProfile(socket, data); return; }
    else if (action == "UPDATE_PROFILE") { handleUpdateProfile(socket, data); return; }
    else if (action == "CHANGE_PASSWORD") { handleChangePassword(socket, data); return; }
    else if (action == "GET_PURCHASE_HISTORY") { handleGetPurchaseHistory(socket, data); return; }
    else if (action == "CHECK_BOOK_OWNERSHIP") { handleCheckBookOwnership(socket, data); return; }
    else if (action == "GET_BOOK_PDF_PATH") { handleGetBookPdfPath(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 2 )***************************************************

    else if (action == "SEARCH_BOOKS") { handleSearchBooks(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 3 )***************************************************

    else if (action == "ADD_COMMENT") { handleAddComment(socket, data); return; }
    else if (action == "EDIT_COMMENT") { handleEditComment(socket, data); return; }
    else if (action == "DELETE_COMMENT") { handleDeleteComment(socket, data); return; }
    else if (action == "GET_COMMENTS") { handleGetComments(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 4 )***************************************************

    else if (action == "ADD_TO_CART") { handleAddToCart(socket, data); return; }
    else if (action == "REMOVE_FROM_CART") { handleRemoveFromCart(socket, data); return; }
    else if (action == "GET_CART") { handleGetCart(socket, data); return; }
    else if (action == "FINALIZE_PURCHASE") { handleFinalizePurchase(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 5 )***************************************************

    //+++++کتاب های خریداری شده+++++
    else if (action == "GET_PURCHASED_BOOKS") { handleGetPurchasedBooks(socket, data); return; }

    //+++++کتاب های ذخیره شده+++++
    else if (action == "SAVE_BOOK") { handleSaveBook(socket, data); return; }
    else if (action == "REMOVE_SAVED_BOOK") { handleRemoveSavedBook(socket, data); return; }
    else if (action == "GET_SAVED_BOOKS") { handleGetSavedBooks(socket, data); return; }

    //+++++قفسه ها+++++
    else if (action == "CREATE_SHELF") { handleCreateShelf(socket, data); return; }
    else if (action == "RENAME_SHELF") { handleRenameShelf(socket, data); return; }
    else if (action == "DELETE_SHELF") { handleDeleteShelf(socket, data); return; }
    else if (action == "ADD_BOOK_TO_SHELF") { handleAddBookToShelf(socket, data); return; }
    else if (action == "MOVE_BOOK_BETWEEN_SHELVES") { handleMoveBookBetweenShelves(socket, data); return; }
    else if (action == "GET_SHELVES") { handleGetShelves(socket, data); return; }
    else if (action == "GET_SHELF_BOOKS") { handleGetShelfBooks(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 6 )****************************************************

    else if (action == "GET_LAST_READ_PAGE") { handleGetLastReadPage(socket, data); return; }
    else if (action == "UPDATE_LAST_READ_PAGE") { handleUpdateLastReadPage(socket, data); return; }



}

//--------------------------------------------------------------------------------------------------------------------------
//                                               ***پیاده سازی متد ها***
// -------------------------------------------------------------------------------------------------------------------------

//***************************************************احراز هویت مرکزی******************************************************

void NetworkWorker::handleLogin(QTcpSocket* socket, const QJsonObject& data) {
    QString username = data.value("username").toString();
    QString passwordPlain = data.value("password").toString();

    UserRole role; bool isBlocked; int userId = 0; int firstLogin;
    QJsonObject resp;
    resp["action"] = "LOGIN_RESPONSE";

    if (!m_dbManager.verifyUser(username, passwordPlain, role, isBlocked, userId, firstLogin)) {
        resp["status"] = "FAILED";
        resp["message"] = ".نام کاربری یا رمز عبور اشتباه است یا حساب شما مسدود است";
        sendJson(socket, resp);
        return;
    }

    resp["status"] = "SUCCESS";
    resp["message"] = "!خوش آمدی";
    resp["user_role"] = roleToString(role);
    resp["first_login"] = firstLogin; // فرستادن وضعیت به کلاینت (۱ یعنی برو صفحه ژانر، ۰ یعنی برو صفحه اصلی)
    resp["user_id"] = userId;

    sendJson(socket, resp);

    // شلیک سیگنال ورود به سرور
    emit userLoggedIn(userId, username, socket);

}

void NetworkWorker::handleRegister(QTcpSocket* socket, const QJsonObject& data) {
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

    if (m_dbManager.isUsernameTaken(username)) {
        resp["status"] = "FAILED";
        resp["message"] = ".نام کاربری تکراری است";
        sendJson(socket, resp);
        return;
    }
    if (!m_dbManager.registerUser(username, passwordPlain, role, securityQuestion, securityAnswerPlain)) {
        resp["status"] = "FAILED";
        resp["message"] = ".خطا در ثبت نام";
        sendJson(socket, resp);
        return;
    }

    resp["status"] = "SUCCESS";
    resp["message"] = ".ثبت نام با موفقیت انجام شد";
    sendJson(socket, resp);
}

void NetworkWorker::handleForgotPassword(QTcpSocket* socket, const QJsonObject& data) {
    QString step = data.value("step").toString();
    QJsonObject resp;

    if (step == "REQUEST_QUESTION") {
        QString username = data.value("username").toString();
        QString question;
        resp["action"] = "FORGOT_PASSWORD_RESPONSE";

        if (!m_dbManager.getSecurityQuestion(username, question)) {
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

        if (!m_dbManager.verifySecurityAnswerAndResetPassword(username, answerPlain, newPasswordPlain)) {
            resp["status"] = "FAILED";
            resp["message"] = ".پاسخ امنیتی نادرست است";
        } else {
            resp["status"] = "SUCCESS";
            resp["message"] = ".رمز عبور با موفقیت تغییر کرد";
        }
        sendJson(socket, resp);
    }
}

//*********************************************پنل کاربر عادی ( ماژول 1 ) *************************************************

void NetworkWorker::handleSetFavoriteGenres(QTcpSocket* socket, const QJsonObject& data) {
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

    bool ok = m_dbManager.setFavoriteGenres(username, genres);

    if (ok) {
        m_dbManager.setFirstLoginFalseByUsername(username);
    }

    QJsonObject resp;
    resp["action"] = "SET_FAVORITE_GENRES_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".ژانرهای مورد علاقه با موفقیت ذخیره شدند"
                         : ".خطا در ذخیره ژانرهای مورد علاقه";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetRecommendedBooks(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    QStringList genres = m_dbManager.getFavoriteGenres(username);
    const QList<QJsonObject> books = m_dbManager.getRecommendedBooks(genres);

    QJsonArray arr;
    QString baseDir = QCoreApplication::applicationDirPath();

    for (const QJsonObject& b : std::as_const(books)) {
        // ایجاد یک کپی موقت و قابل تغییر برای اصلاح مسیر
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = mutableBook.value("coverImagePath").toString();

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath);
        } else {
            mutableBook["cover_image_path"] = "";
        }

        arr.append(mutableBook); // اضافه کردن شیء اصلاح شده به آرایه
    }

    QJsonObject resp;
    resp["action"] = "GET_RECOMMENDED_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

void NetworkWorker::handleGetBooksByGenre(QTcpSocket* socket, const QJsonObject& data) {
    const QString genre = data.value("genre").toString();
    const QList<QJsonObject> books = m_dbManager.getBooksByGenre(genre);

    QJsonArray arr;
    QString baseDir = QCoreApplication::applicationDirPath();

    for (const QJsonObject& b : std::as_const(books)) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = mutableBook.value("coverImagePath").toString();

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath);
        } else {
            mutableBook["cover_image_path"] = "";
        }

        arr.append(mutableBook);
    }

    QJsonObject resp;
    resp["action"] = "GET_BOOKS_BY_GENRE_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

void NetworkWorker::handleGetPopularBooks(QTcpSocket* socket) {
    const QList<QJsonObject> books = m_dbManager.getPopularBooks();
    QJsonArray arr;
    QString baseDir = QCoreApplication::applicationDirPath();

    for (const QJsonObject& b : std::as_const(books)) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = mutableBook.value("coverImagePath").toString();

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath);
        } else {
            mutableBook["cover_image_path"] = "";
        }

        arr.append(mutableBook);
    }

    QJsonObject resp;
    resp["action"] = "GET_POPULAR_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

void NetworkWorker::handleGetNewBooks(QTcpSocket* socket) {
    const QList<QJsonObject> books = m_dbManager.getNewBooks();
    QJsonArray arr;
    QString baseDir = QCoreApplication::applicationDirPath();

    for (const QJsonObject& b : std::as_const(books)) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = mutableBook.value("coverImagePath").toString();

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath);
        } else {
            mutableBook["cover_image_path"] = "";
        }

        arr.append(mutableBook);
    }

    QJsonObject resp;
    resp["action"] = "GET_NEW_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

void NetworkWorker::handleGetBestsellers(QTcpSocket* socket) {
    const QList<QJsonObject> books = m_dbManager.getBestsellers();
    QJsonArray arr;
    QString baseDir = QCoreApplication::applicationDirPath();

    for (const QJsonObject& b : std::as_const(books)) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = mutableBook.value("coverImagePath").toString();

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath);
        } else {
            mutableBook["cover_image_path"] = "";
        }

        arr.append(mutableBook);
    }

    QJsonObject resp;
    resp["action"] = "GET_BESTSELLERS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

void NetworkWorker::handleGetFreeBooks(QTcpSocket* socket) {
    const QList<QJsonObject> books = m_dbManager.getFreeBooks();
    QJsonArray arr;
    QString baseDir = QCoreApplication::applicationDirPath();

    for (const QJsonObject& b : std::as_const(books)) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) coverPath = mutableBook.value("coverImagePath").toString();

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath);
        } else {
            mutableBook["cover_image_path"] = "";
        }

        arr.append(mutableBook);
    }

    QJsonObject resp;
    resp["action"] = "GET_FREE_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}

void NetworkWorker::handleGetProfile(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    QJsonObject profile = m_dbManager.getUserProfile(username);

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

void NetworkWorker::handleUpdateProfile(QTcpSocket* socket, const QJsonObject& data) {
    const int userId = data.value("user_id").toInt();
    const QString newUsername = data.value("username").toString().trimmed();
    const QString name = data.value("name").toString().trimmed();
    const QString email = data.value("email").toString().trimmed();

    QJsonObject resp;
    resp["action"] = "UPDATE_PROFILE_RESPONSE";

    if (newUsername.isEmpty()) {
        resp["status"] = "FAILED";
        resp["message"] = ".نام کاربری (یوزرنیم) نمی‌تواند خالی باشد";
        sendJson(socket, resp);
        return;
    }

    bool ok = m_dbManager.updateUserProfile(userId, newUsername, name, email);

    if (ok) {
        resp["status"] = "SUCCESS";
        resp["message"] = ".اطلاعات حساب کاربری با موفقیت به روزرسانی شد";
    } else {
        resp["status"] = "FAILED";
        resp["message"] = ".این نام کاربری یا ایمیل قبلاً توسط شخص دیگری انتخاب شده است";
    }

    sendJson(socket, resp);
}

void NetworkWorker::handleChangePassword(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    const QString oldPass = data.value("old_password").toString();
    const QString newPass = data.value("new_password").toString();

    bool ok = m_dbManager.changePassword(username, oldPass, newPass);

    QJsonObject resp;
    resp["action"] = "CHANGE_PASSWORD_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".رمز عبور با موفقیت تغییر کرد "
                         : ".رمز عبور فعلی اشتباه است یا کاربر یافت نشد";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetPurchaseHistory(QTcpSocket* socket, const QJsonObject& data) {
    const QString username = data.value("username").toString();
    QList<QJsonObject> history = m_dbManager.getPurchaseHistory(username);

    QJsonArray arr;
    for (const QJsonObject& h : std::as_const(history))
        arr.append(h);

    QJsonObject resp;
    resp["action"] = "GET_PURCHASE_HISTORY_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["history"] = arr;
    sendJson(socket, resp);
}

// بررسی وضعیت خرید و فرستادن آدرس کامل فیزیکی عکس کاور
void NetworkWorker::handleCheckBookOwnership(QTcpSocket* socket, const QJsonObject& data)
{
    int userId = data.value("user_id").toInt();
    int bookId = data.value("book_id").toInt();

    QString publisher;
    double rating = 0.0;
    QString coverPath;

    QJsonObject resp;
    resp["action"] = "CHECK_BOOK_OWNERSHIP_RESPONSE";
    resp["book_id"] = bookId;

    if (!m_dbManager.getActiveBookDetails(bookId, publisher, rating, coverPath)) {
        resp["status"] = "FAILED";
        resp["message"] = ".این کتاب در حال حاضر غیرفعال یا ناموجود است";
        sendJson(socket, resp);
        return;
    }

    bool purchased = m_dbManager.isBookPurchased(userId, bookId);

    resp["status"] = "SUCCESS";
    resp["is_purchased"] = purchased;
    resp["publisher_name"] = publisher;
    resp["rating"] = rating;

    // اعمال فرمول روی مسیر عکس کاور
    if (!coverPath.isEmpty()) {
        QString baseDir = QCoreApplication::applicationDirPath();
        resp["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath); // ارسال آدرس کامل هارد
    } else {
        resp["cover_image_path"] = "";
    }

    sendJson(socket, resp);
}

// فرستادن آدرس کامل فیزیکی پی‌دی‌اف برای مطالعه/دانلود کلاینت
void NetworkWorker::handleGetBookPdfPath(QTcpSocket* socket, const QJsonObject& data)
{
    int userId = data.value("user_id").toInt();
    int bookId = data.value("book_id").toInt();

    QJsonObject resp;
    resp["action"] = "GET_BOOK_PDF_PATH_RESPONSE";
    resp["book_id"] = bookId;

    // لایه امنیتی: حتماً چک شود که کاربر کتاب را خریده باشد
    if (!m_dbManager.isBookPurchased(userId, bookId)) {
        resp["status"] = "FAILED";
        resp["message"] = ".شما دسترسی به این کتاب ندارید. ابتدا باید آن را خریداری کنید";
        sendJson(socket, resp);
        return;
    }

    // گرفتن آدرس نسبی از دیتابیس
    QString pdfPath = m_dbManager.getBookPdfPath(bookId);

    if (!pdfPath.isEmpty()) {
        resp["status"] = "SUCCESS";
        // اعمال فرمول جدید روی مسیر پی‌دی‌اف
        QString baseDir = QCoreApplication::applicationDirPath();
        resp["pdf_path"] = QDir::cleanPath(baseDir + "/" + pdfPath); // ارسال آدرس کامل هارد برای کپی مستقیم کلاینت
    } else {
        resp["status"] = "FAILED";
        resp["message"] = ".فایل پی‌دی‌اف این کتاب یافت نشد";
    }

    sendJson(socket, resp);
}

//*********************************************پنل کاربر عادی ( ماژول 2 ) *************************************************

void NetworkWorker::handleSearchBooks(QTcpSocket* socket, const QJsonObject& data)
{
    QString title = data.value("title").toString();
    QString author = data.value("author").toString();
    QString publisher = data.value("publisher_name").toString();

    // دریافت نتایج جستجو از دیتابیس
    const QList<QJsonObject> books = m_dbManager.searchBooks(title, author, publisher);
    QJsonArray finalArray;

    // پیدا کردن مسیر پوشه اجرایی سرور برای سرهم کردن آدرس کامل
    QString baseDir = QCoreApplication::applicationDirPath();

    for (const QJsonObject& book : std::as_const(books)) {

        QJsonObject mutableBook = book;


        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = QDir::cleanPath(baseDir + "/" + coverPath);
        } else {
            mutableBook["cover_image_path"] = "";
        }


        finalArray.append(mutableBook);
    }

    QJsonObject resp;
    resp["action"] = "SEARCH_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = finalArray;

    sendJson(socket, resp);
}

//*********************************************پنل کاربر عادی ( ماژول 3 ) *************************************************

void NetworkWorker::handleAddComment(QTcpSocket* socket, const QJsonObject& data) {
    int bookId = data["book_id"].toInt();
    int userId = data["user_id"].toInt();
    QString text = data["text"].toString();
    int rating = data["rating"].toInt();

    bool ok = m_dbManager.addComment(bookId, userId, text, rating);

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
        emit broadcastRequested(b);
    }
}

void NetworkWorker::handleEditComment(QTcpSocket* socket, const QJsonObject& data) {
    int commentId = data["comment_id"].toInt();
    QString text = data["text"].toString();
    int rating = data["rating"].toInt();

    bool ok = m_dbManager.editComment(commentId, text, rating);

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
        emit broadcastRequested(b);
    }
}

void NetworkWorker::handleDeleteComment(QTcpSocket* socket, const QJsonObject& data) {
    int commentId = data["comment_id"].toInt();

    bool ok = m_dbManager.deleteComment(commentId);

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
        emit broadcastRequested(b);
    }
}

void NetworkWorker::handleGetComments(QTcpSocket* socket, const QJsonObject& data) {
    int bookId = data["book_id"].toInt();

    QList<QJsonObject> list = m_dbManager.getCommentsForBook(bookId);

    QJsonArray arr;
    for (auto &c : list)
        arr.append(c);

    QJsonObject resp;
    resp["action"] = "GET_COMMENTS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["comments"] = arr;
    sendJson(socket, resp);
}

//*********************************************پنل کاربر عادی ( ماژول 4 ) *************************************************

void NetworkWorker::handleAddToCart(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = m_dbManager.addToCart(userId, bookId);

    QJsonObject resp;
    resp["action"] = "ADD_TO_CART_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".کتاب به سبد خرید اضافه شد"
                         : ".خطا در افزودن کتاب";

    sendJson(socket, resp);
}

void NetworkWorker::handleRemoveFromCart(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = m_dbManager.removeFromCart(userId, bookId);

    QJsonObject resp;
    resp["action"] = "REMOVE_FROM_CART_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".کتاب از سبد خرید حذف شد"
                         : ".خطا در حذف کتاب";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetCart(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    QList<QJsonObject> items = m_dbManager.getCartItems(userId);

    QJsonArray arr;
    double total = 0;
    double discountTotal = 0;

    //پیدا کردن مسیر پوشه اجرایی سرور
    QString baseDir = QCoreApplication::applicationDirPath();

    for (auto i : items) {
        double price = i["price"].toDouble();
        double discount = i["discount"].toDouble();

        discountTotal += price * (discount / 100.0);
        total += price;

        QString relativeCover = i["coverImagePath"].toString();
        if (!relativeCover.isEmpty()) {
            i["coverImagePath"] = QDir::cleanPath(baseDir + "/" + relativeCover);
        }

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

void NetworkWorker::handleFinalizePurchase(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    bool ok = m_dbManager.finalizePurchase(userId);

    QJsonObject resp;
    resp["action"] = "FINALIZE_PURCHASE_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".خرید با موفقیت انجام شد"
                         : ".خطا در نهایی سازی خرید";

    sendJson(socket, resp);
}

//*********************************************پنل کاربر عادی ( ماژول 5 ) *************************************************

//+++++کتاب های خریداری شده+++++
void NetworkWorker::handleGetPurchasedBooks(QTcpSocket* socket, const QJsonObject& data)
{
    int userId = data.value("user_id").toInt();
    QJsonObject resp;
    resp["action"] = "GET_PURCHASED_BOOKS_RESPONSE";

    if (userId <= 0) {
        resp["status"] = "FAILED";
        resp["message"] = ".شناسه کاربر نامعتبر است";
        sendJson(socket, resp);
        return;
    }

    // دریافت لیست کتاب ها از دیتابیس
    QList<QJsonObject> purchasedList = m_dbManager.getPurchasedBooks(userId);
    QJsonArray finalArray;

    //پیدا کردن مسیر پوشه اجرایی سرور
    QString baseDir = QCoreApplication::applicationDirPath();

    for (auto book : purchasedList) {

        // --- اعمال فرمول جدید روی مسیر عکس کاور ---
        QString relativeCover = book["coverImagePath"].toString();
        if (!relativeCover.isEmpty()) {
            book["coverImagePath"] = QDir::cleanPath(baseDir + "/" + relativeCover);
        }

        // --- اعمال فرمول روی مسیر فایل کتاب ---
        QString relativePdf = book["pdfPath"].toString();
        if (!relativePdf.isEmpty()) {
            book["pdfPath"] = QDir::cleanPath(baseDir + "/" + relativePdf);
        }

        finalArray.append(book);
    }

    resp["status"] = "SUCCESS";
    resp["books"] = finalArray;
    sendJson(socket, resp);
}

//+++++کتاب های ذخیره شده+++++
void NetworkWorker::handleSaveBook(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = m_dbManager.saveBook(userId, bookId);

    QJsonObject resp;
    resp["action"] = "SAVE_BOOK_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".کتاب ذخیره شد"
                         : ".خطا در ذخیره کتاب";

    sendJson(socket, resp);
}

void NetworkWorker::handleRemoveSavedBook(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = m_dbManager.removeSavedBook(userId, bookId);

    QJsonObject resp;
    resp["action"] = "REMOVE_SAVED_BOOK_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".کتاب از لیست ذخیره شده حذف شد"
                         : ".خطا در حذف کتاب";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetSavedBooks(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    QList<QJsonObject> list = m_dbManager.getSavedBooks(userId);
    QJsonArray arr;

    //پیدا کردن مسیر پوشه اجرایی سرور
    QString baseDir = QCoreApplication::applicationDirPath();

    for (auto b : list) {
        QString relativeCover = b["coverImagePath"].toString();

        // ۲ و ۳. فرمول جدید برای ساخت آدرس فیزیکی کامل و بدون باگ روی هارد
        if (!relativeCover.isEmpty()) {
            b["coverImagePath"] = QDir::cleanPath(baseDir + "/" + relativeCover);
        }
        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_SAVED_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;

    sendJson(socket, resp);
}

//+++++قفسه ها+++++
void NetworkWorker::handleCreateShelf(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    QString name = data["name"].toString();

    bool ok = m_dbManager.createShelf(userId, name);

    QJsonObject resp;
    resp["action"] = "CREATE_SHELF_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".قفسه با موفقیت ایجاد شد"
                         : ".قفسه ای با این نام از قبل وجود دارد یا خطا رخ داده است";

    sendJson(socket, resp);
}

void NetworkWorker::handleRenameShelf(QTcpSocket* socket, const QJsonObject& data) {
    int shelfId = data["shelf_id"].toInt();
    QString newName = data["new_name"].toString();

    bool ok = m_dbManager.renameShelf(shelfId, newName);

    QJsonObject resp;
    resp["action"] = "RENAME_SHELF_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".نام قفسه تغییر کرد"
                         : ".این نام با یکی از قفسه های دیگر شما تداخل دارد یا قفسه یافت نشد";

    sendJson(socket, resp);
}

void NetworkWorker::handleDeleteShelf(QTcpSocket* socket, const QJsonObject& data) {
    int shelfId = data["shelf_id"].toInt();

    bool ok = m_dbManager.deleteShelf(shelfId);

    QJsonObject resp;
    resp["action"] = "DELETE_SHELF_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".قفسه حذف شد"
                         : ".خطا در حذف قفسه";

    sendJson(socket, resp);
}

void NetworkWorker::handleAddBookToShelf(QTcpSocket* socket, const QJsonObject& data) {
    int shelfId = data["shelf_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = m_dbManager.addBookToShelf(shelfId, bookId);

    QJsonObject resp;
    resp["action"] = "ADD_BOOK_TO_SHELF_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".کتاب به قفسه اضافه شد"
                         : ".این کتاب از قبل در قفسه مورد نظر موجود است";

    sendJson(socket, resp);
}

void NetworkWorker::handleMoveBookBetweenShelves(QTcpSocket* socket, const QJsonObject& data) {
    int fromShelf = data["from_shelf"].toInt();
    int toShelf = data["to_shelf"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = m_dbManager.moveBookBetweenShelves(fromShelf, toShelf, bookId);

    QJsonObject resp;
    resp["action"] = "MOVE_BOOK_BETWEEN_SHELVES_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".کتاب با موفقیت منتقل شد"
                         : ".کتاب از قبل در قفسه مقصد موجود بود و از قفسه فعلی حذف شد";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetShelves(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    QList<QJsonObject> list = m_dbManager.getShelves(userId);

    QJsonArray arr;
    for (auto &o : list)
        arr.append(o);

    QJsonObject resp;
    resp["action"] = "GET_SHELVES_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["shelves"] = arr;

    sendJson(socket, resp);
}

void NetworkWorker::handleGetShelfBooks(QTcpSocket* socket, const QJsonObject& data) {
    int shelfId = data["shelf_id"].toInt();

    QList<QJsonObject> list = m_dbManager.getBooksInShelf(shelfId);
    QJsonArray arr;

    // پیدا کردن مسیر کامل پوشه اجرایی سرور روی هارد
    QString baseDir = QCoreApplication::applicationDirPath();

    for (auto b : list) { // رفرنس (&) را برداشتیم تا کپی شیء را ویرایش کنیم
        QString relativeCover = b["coverImagePath"].toString();

        // اگر کتاب کاور داشت، مسیر آن را به آدرس کامل فیزیکی تبدیل کن
        if (!relativeCover.isEmpty()) {
            b["coverImagePath"] = QDir::cleanPath(baseDir + "/" + relativeCover);
        }

        arr.append(b);
    }

    QJsonObject resp;
    resp["action"] = "GET_SHELF_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;

    sendJson(socket, resp);
}

//*********************************************پنل کاربر عادی ( ماژول 6 ) *************************************************

void NetworkWorker::handleGetLastReadPage(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    int page = m_dbManager.getLastReadPage(userId, bookId);

    QJsonObject resp;
    resp["action"] = "GET_LAST_READ_PAGE_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["page"] = page;

    sendJson(socket, resp);
}

void NetworkWorker::handleUpdateLastReadPage(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();
    int page = data["page"].toInt();

    bool ok = m_dbManager.updateLastReadPage(userId, bookId, page);

    QJsonObject resp;
    resp["action"] = "UPDATE_LAST_READ_PAGE_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".آخرین صفحه ذخیره شد"
                         : ".خطا در ذخیره صفحه";

    sendJson(socket, resp);
}


