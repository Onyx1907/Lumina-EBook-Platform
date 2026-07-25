#include "networkworker.h"


//تابع کمکی استاتیک
static QString roleToString(UserRole role) {
    switch (role) {
    case UserRole::RegularUser: return "RegularUser";
    case UserRole::Publisher:   return "Publisher";
    case UserRole::Admin:       return "Admin";
    }
    return "RegularUser";
}

NetworkWorker::NetworkWorker(qintptr socketDescriptor, DatabaseManager* db)
    : m_socketDescriptor(socketDescriptor),
    m_socket(nullptr),
    m_dbManager(db)
{
    qRegisterMetaType<QTcpSocket*>("QTcpSocket*");
}
void NetworkWorker::startProcessing() {
    m_socket = new QTcpSocket();
    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        emit finished();
        return;
    }
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkWorker::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkWorker::onDisconnected);
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
    //ابتدا به سرور خبر میدهیم که این سوکت دیسکانکت شد تا از مپ آنلاین ها حذف شود
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

    // اضافه کردن کاراکتر \n به انتهای پیام تا کلاینت هم بتواند آن را خط به خط بخواند
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

    else if (action == "ADD_COMMENT") { handleAddComment(socket, obj); return; }
    else if (action == "EDIT_COMMENT") { handleEditComment(socket, obj); return; }
    else if (action == "DELETE_COMMENT") { handleDeleteComment(socket, obj); return; }
    else if (action == "GET_COMMENTS") { handleGetComments(socket, obj); return; }


    //*********************************************پنل کاربر عادی ( ماژول 4 )***************************************************

    else if (action == "ADD_TO_CART") { handleAddToCart(socket, data); return; }
    else if (action == "REMOVE_FROM_CART") { handleRemoveFromCart(socket, data); return; }
    else if (action == "GET_CART") { handleGetCart(socket, data); return; }
    else if (action == "FINALIZE_PURCHASE") { handleFinalizePurchase(socket, data); return; }


    //*********************************************پنل کاربر عادی ( ماژول 5 )***************************************************

    //+++++کتاب های خریداری شده+++++
    else if (action == "GET_PURCHASED_BOOKS") { handleGetPurchasedBooks(socket, data); return; }

    //+++++کتاب های ذخیره شده+++++
    else if (action == "SAVE_BOOK") { handleSaveBook(socket, obj); return; }
    else if (action == "REMOVE_SAVED_BOOK") { handleRemoveSavedBook(socket, obj); return; }
    else if (action == "GET_SAVED_BOOKS") { handleGetSavedBooks(socket, data); return; }

    //+++++قفسه ها+++++
    else if (action == "CREATE_SHELF") { handleCreateShelf(socket, obj); return; }
    else if (action == "RENAME_SHELF") { handleRenameShelf(socket, obj); return; }
    else if (action == "DELETE_SHELF") { handleDeleteShelf(socket, obj); return; }
    else if (action == "ADD_BOOK_TO_SHELF") { handleAddBookToShelf(socket, obj); return; }
    else if (action == "MOVE_BOOK_BETWEEN_SHELVES") { handleMoveBookBetweenShelves(socket, obj); return; }
    else if (action == "GET_SHELVES") { handleGetShelves(socket, data); return; }
    else if (action == "GET_SHELF_BOOKS") { handleGetShelfBooks(socket, obj); return; }


    //*********************************************پنل کاربر عادی ( ماژول 6 )****************************************************

    else if (action == "GET_LAST_READ_PAGE") { handleGetLastReadPage(socket, obj); return; }
    else if (action == "UPDATE_LAST_READ_PAGE") { handleUpdateLastReadPage(socket, obj); return; }


    //************************************************پنل ناشر ( ماژول 1 )*******************************************************

    else if (action == "GET_PUBLISHER_PROFILE") { handleGetPublisherProfile(socket, obj); return; }
    else if (action == "UPDATE_PUBLISHER_PROFILE") { handleUpdatePublisherProfile(socket, obj); return; }


    //************************************************پنل ناشر ( ماژول 2 )*******************************************************

    else if (action == "ADD_BOOK") { handleAddBook(socket, obj); return; }
    else if (action == "UPDATE_BOOK") { handleUpdateBook(socket, obj); return; }
    else if (action == "SET_BOOK_DISCOUNT") { handleSetBookDiscount(socket, obj); return; }
    else if (action == "SET_BOOK_ACTIVE_STATE") { handleSetBookActiveState(socket, obj); return; }
    else if (action == "PUBLISHER_DELETE_BOOK") { handlePublisherDeleteBook(socket, obj); return; }
    else if (action == "GET_PUBLISHER_BOOKS")  { handleGetPublisherBooks(socket, obj); return; }


    //************************************************پنل ناشر ( ماژول 3 )*******************************************************

    else if (action == "GET_PUBLISHER_STATS") { handleGetPublisherStats(socket, obj); return; }


    //*********************************************پنل مدیر سیستم ( ماژول 1 )****************************************************

    else if (action == "GET_ALL_USERS") { handleGetAllUsers(socket, data); return; }
    else if (action == "GET_USER_DETAILS") { handleGetUserDetails(socket, data); return; }
    else if (action == "SEARCH_USERS") { handleSearchUsers(socket, data); return; }


    //*********************************************پنل مدیر سیستم ( ماژول 2 )****************************************************

    else if (action == "DELETE_USER") { handleDeleteUser(socket, data); return; }
    else if (action == "SET_USER_ACTIVE_STATE") { handleSetUserActiveState(socket, data); return; }


    //*********************************************پنل مدیر سیستم ( ماژول 3 )****************************************************

    else if (action == "GET_ALL_BOOKS") { handleGetAllBooks(socket, data); return; }
    else if (action == "ADMIN_UPDATE_BOOK") { handleAdminUpdateBook(socket, obj); return; }
    else if (action == "ADMIN_DELETE_BOOK") { handleAdminDeleteBook(socket, obj); return; }


    //****************************************************سیستم اعلان ها**********************************************************

    else if (action == "GET_NOTIFICATIONS") { handleGetNotifications(socket, data); return; }
    else if (action == "MARK_NOTIFICATION_READ") { handleMarkNotificationRead(socket, data); return; }





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

    QString inputHash = CryptoHelper::hashPassword(passwordPlain);
    if (username == ADMIN_USERNAME && inputHash == ADMIN_PASSWORD_HASH) {
        resp["status"] = "SUCCESS";
        resp["message"] = "!خوش آمدی مدیر";
        resp["user_role"] = "Admin";
        resp["first_login"] = 0;
        resp["user_id"] = -1;

        sendJson(socket, resp);
        emit userLoggedIn(-1, username, socket);
        return;
    }

    if(!m_dbManager->verifyUser(username, passwordPlain, role, isBlocked, userId, firstLogin)) {
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

    if (m_dbManager->isUsernameTaken(username)) {
        resp["status"] = "FAILED";
        resp["message"] = ".نام کاربری تکراری است";
        sendJson(socket, resp);
        return;
    }
    if (!m_dbManager->registerUser(username, passwordPlain, role, securityQuestion, securityAnswerPlain)) {
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

        if (!m_dbManager->getSecurityQuestion(username, question)) {
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

        if (!m_dbManager->verifySecurityAnswerAndResetPassword(username, answerPlain, newPasswordPlain)) {
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

    bool ok = m_dbManager->setFavoriteGenres(username, genres);

    if (ok) {
        m_dbManager->setFirstLoginFalseByUsername(username);
    }

    QJsonObject resp;
    resp["action"] = "SET_FAVORITE_GENRES_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".ژانرهای مورد علاقه با موفقیت ذخیره شدند"
                         : ".خطا در ذخیره ژانرهای مورد علاقه";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetRecommendedBooks(QTcpSocket* socket, const QJsonObject& data) {
    const int userId = data.value("user_id").toInt();
    QStringList genres = m_dbManager->getFavoriteGenres(userId);
    QList<QJsonObject> books = m_dbManager->getRecommendedBooks(genres);

    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی کامل با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &b : books) {
        // ایجاد یک کپی موقت و قابل تغییر برای اصلاح مسیر
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = storagePath + coverPath;
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
    QList<QJsonObject> books = m_dbManager->getBooksByGenre(genre);

    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی کامل با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &b : books) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = storagePath + coverPath;
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
    QList<QJsonObject> books = m_dbManager->getPopularBooks();
    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی کامل با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &b : books) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = storagePath + coverPath;
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
    QList<QJsonObject> books = m_dbManager->getNewBooks();
    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &b : books) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = storagePath + coverPath;
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
    QList<QJsonObject> books = m_dbManager->getBestsellers();
    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &b : books) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = storagePath + coverPath;
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
    const QList<QJsonObject> books = m_dbManager->getFreeBooks();
    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &b : books) {
        QJsonObject mutableBook = b;

        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = storagePath + coverPath;
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
    const int userId = data.value("user_id").toInt();
    QJsonObject profile = m_dbManager->getUserProfile(userId);

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

void NetworkWorker::handleUpdateProfile(QTcpSocket* socket, const QJsonObject& requestDoc) {
    // حل مشکل اصلی: ورود به لایه data برای استخراج درست مقادیر
    QJsonObject dataObj = requestDoc.value("data").toObject();

    // اگر کلاینت ساختار data را نفرستاده باشد، از خود شیء اصلی استفاده میکند
    if (dataObj.isEmpty()) {
        dataObj = requestDoc;
    }

    const int userId = dataObj.value("user_id").toInt();
    const QString newUsername = dataObj.value("username").toString().trimmed();
    const QString name = dataObj.value("name").toString().trimmed();
    const QString email = dataObj.value("email").toString().trimmed();

    QJsonObject resp;
    resp["action"] = "UPDATE_PROFILE_RESPONSE";

    // اجرای تابع دیتابیس با منطق جدید و منعطف
    bool ok = m_dbManager->updateUserProfile(userId, newUsername, name, email);

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
    const int userId = data.value("user_id").toInt();
    const QString oldPass = data.value("old_password").toString();
    const QString newPass = data.value("new_password").toString();

    bool ok = m_dbManager->changePassword(userId, oldPass, newPass);

    QJsonObject resp;
    resp["action"] = "CHANGE_PASSWORD_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".رمز عبور با موفقیت تغییر کرد "
                         : ".رمز عبور فعلی اشتباه است یا کاربر یافت نشد";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetPurchaseHistory(QTcpSocket* socket, const QJsonObject& data) {
    const int userId = data.value("user_id").toInt();
    QList<QJsonObject> history = m_dbManager->getPurchaseHistory(userId);
    int totalCount = m_dbManager->getTotalPurchases(userId);

    QJsonArray arr;
    for (const QJsonObject& h : history)
        arr.append(h);

    QJsonObject resp;
    resp["action"] = "GET_PURCHASE_HISTORY_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["total_purchases"] = totalCount;
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
    QString description;

    QJsonObject resp;
    resp["action"] = "CHECK_BOOK_OWNERSHIP_RESPONSE";
    resp["book_id"] = bookId;

    if (!m_dbManager->getActiveBookDetails(bookId, publisher, rating, coverPath, description)) {
        resp["status"] = "FAILED";
        resp["message"] = ".این کتاب در حال حاضر غیرفعال یا ناموجود است";
        sendJson(socket, resp);
        return;
    }

    bool purchased = m_dbManager->isBookPurchased(userId, bookId);
    bool inCart = m_dbManager->isBookInCart(userId, bookId);
    bool isSaved = m_dbManager->isBookSaved(userId, bookId);

    resp["status"] = "SUCCESS";
    resp["is_purchased"] = purchased;
    resp["is_in_cart"] = inCart;
    resp["is_saved"] = isSaved;
    resp["publisher_name"] = publisher;
    resp["rating"] = rating;
    resp["description"] = description;

    // اعمال فرمول روی مسیر عکس کاور با استفاده از مسیر استاندارد Home
    if (!coverPath.isEmpty()) {
        QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";
        resp["cover_image_path"] = storagePath + coverPath; // ارسال آدرس کامل هارد طبق منطق اینستالر
    } else {
        resp["cover_image_path"] = "";
    }

    sendJson(socket, resp);
}

// فرستادن آدرس کامل فیزیکی پی دی اف برای مطالعه/دانلود کلاینت
void NetworkWorker::handleGetBookPdfPath(QTcpSocket* socket, const QJsonObject& data)
{
    int userId = data.value("user_id").toInt();
    int bookId = data.value("book_id").toInt();

    QJsonObject resp;
    resp["action"] = "GET_BOOK_PDF_PATH_RESPONSE";
    resp["book_id"] = bookId;

    // لایه امنیتی: حتماً چک شود که کاربر کتاب را خریده باشد
    if (!m_dbManager->isBookPurchased(userId, bookId)) {
        resp["status"] = "FAILED";
        resp["message"] = ".شما دسترسی به این کتاب ندارید. ابتدا باید آن را خریداری کنید";
        sendJson(socket, resp);
        return;
    }

    QString pdfPath = m_dbManager->getBookPdfPath(bookId);

    if (!pdfPath.isEmpty()) {
        resp["status"] = "SUCCESS";

        // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
        QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

        // ارسال آدرس کامل (مسیر Home + نام فایل) برای کلاینت
        resp["pdf_path"] = storagePath + pdfPath;
    } else {
        resp["status"] = "FAILED";
        resp["message"] = ".فایل پی دی اف این کتاب یافت نشد";
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
    QList<QJsonObject> books = m_dbManager->searchBooks(title, author, publisher);
    QJsonArray finalArray;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &book : books) {
        QJsonObject mutableBook = book;

        // چک کردن هر دو کلید احتمالی که در دیتابیس یا سمت کلاینت استفاده شده است
        QString coverPath = mutableBook.value("cover_image_path").toString();
        if (coverPath.isEmpty()) {
            coverPath = mutableBook.value("coverImagePath").toString();
        }

        // تبدیل نام فایل به مسیر کامل برای کلاینت
        if (!coverPath.isEmpty()) {
            mutableBook["cover_image_path"] = storagePath + coverPath;
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

    bool ok = m_dbManager->addComment(bookId, userId, text, rating);

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

        //پیدا کردن ناشر و ایجاد اعلان با استفاده از متدهای DatabaseManager (امن برای چندتردی)
        int publisherId = m_dbManager->getPublisherIdForBook(bookId);
        if (publisherId != -1) {
            QString bookTitle = m_dbManager->getBookTitle(bookId);
            QString messageText = QString(".برای کتاب شما (%1) نظر یا امتیاز جدیدی ثبت شد").arg(bookTitle);

            // استفاده از تابع createNotification که از قبل در DatabaseManager داشتید
            int notifId = m_dbManager->createNotification(publisherId, "", "NEW_COMMENT", messageText, bookId);

            if (notifId > 0) {
                //ارسال آنی اعلان به ناشر در صورت آنلاین بودن
                QJsonObject notifObj;
                notifObj["action"] = "NEW_NOTIFICATION_RECEIVED";
                notifObj["id"] = notifId;
                notifObj["type"] = "NEW_COMMENT";
                notifObj["message"] = messageText;
                notifObj["related_id"] = bookId;
                notifObj["is_read"] = 0;
                notifObj["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

                QString targetUsername = m_dbManager->getUsernameById(publisherId);
                emit notificationTriggered(targetUsername, notifObj);
            }
        }
    }
}

void NetworkWorker::handleEditComment(QTcpSocket* socket, const QJsonObject& data) {
    int commentId = data["comment_id"].toInt();
    QString text = data["text"].toString();
    int rating = data["rating"].toInt();

    bool ok = m_dbManager->editComment(commentId, text, rating);

    QJsonObject resp;
    resp["action"] = "EDIT_COMMENT_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".نظر ویرایش شد"
                         : ".خطا در ویرایش";

    sendJson(socket, resp);

    if (ok) {
        int bookId = m_dbManager->getBookIdByCommentId(commentId);

        QJsonObject b;
        b["action"] = "COMMENT_UPDATED";
        b["book_id"] = bookId;
        b["type"] = "EDIT";
        emit broadcastRequested(b);
    }
}

void NetworkWorker::handleDeleteComment(QTcpSocket* socket, const QJsonObject& data) {
    int commentId = data["comment_id"].toInt();

    int bookId = m_dbManager->getBookIdByCommentId(commentId);

    bool ok = m_dbManager->deleteComment(commentId);

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
        b["book_id"] = bookId;
        b["type"] = "DELETE";
        emit broadcastRequested(b);
    }
}

void NetworkWorker::handleGetComments(QTcpSocket* socket, const QJsonObject& data) {
    int bookId = data["book_id"].toInt();

    QList<QJsonObject> list = m_dbManager->getCommentsForBook(bookId);

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

    bool ok = m_dbManager->addToCart(userId, bookId);

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

    bool ok = m_dbManager->removeFromCart(userId, bookId);

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

    QList<QJsonObject> items = m_dbManager->getCartItems(userId);
    QJsonArray arr;
    double total = 0;
    double discountTotal = 0;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &i : items) {
        QJsonObject mutableItem = i;

        double price = mutableItem["price"].toDouble();
        double discount = mutableItem["discount"].toDouble();

        discountTotal += price * (discount / 100.0);
        total += price;

        // تبدیل نام فایل دیتابیس به مسیر کامل برای کلاینت
        QString relativeCover = mutableItem["coverImagePath"].toString();
        if (!relativeCover.isEmpty()) {
            mutableItem["coverImagePath"] = storagePath + relativeCover;
        }

        arr.append(mutableItem);
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

    // دریافت قیمتی که کلاینت در ظاهر اپلیکیشن خود دیده است
    double clientFinalPrice = data["client_final_price"].toDouble();

    //استخراج ناشران و عنوان کتاب ها مستقیماً از دیتابیس (قبل از نهایی شدن خرید و خالی شدن سبد)
    QList<QPair<int, QString>> soldItems = m_dbManager->getPublisherAndBooksForCart(userId);

    // پاس دادن قیمت به دیتابیس جهت صحت سنجی
    bool ok = m_dbManager->finalizePurchase(userId, clientFinalPrice);

    QJsonObject resp;
    resp["action"] = "FINALIZE_PURCHASE_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".خرید با موفقیت انجام شد"
                         : ".قیمت یا موجودی کتاب‌ها تغییر یافته است. سبد خرید شما به‌روزرسانی می‌شود";

    sendJson(socket, resp);

    //ارسال اعلان به ناشران
    if (ok) {
        for (const auto& item : soldItems) {
            int publisherId = item.first;
            QString bookTitle = item.second;

            QString messageText = QString(".کتاب شما (%1) با موفقیت فروخته شد").arg(bookTitle);


            int notifId = m_dbManager->createNotification(publisherId, "", "BOOK_SOLD", messageText, 0);

            if (notifId > 0) {
                QJsonObject notifObj;
                notifObj["action"] = "NEW_NOTIFICATION_RECEIVED";
                notifObj["id"] = notifId;
                notifObj["type"] = "BOOK_SOLD";
                notifObj["message"] = messageText;
                notifObj["is_read"] = 0;
                notifObj["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

                QString targetUsername = m_dbManager->getUsernameById(publisherId);
                emit notificationTriggered(targetUsername, notifObj);
            }
        }
    }
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
    QList<QJsonObject> purchasedList = m_dbManager->getPurchasedBooks(userId);
    QJsonArray finalArray;

    // مسیر استاندارد Home برای اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &book : purchasedList) {
        QJsonObject mutableBook = book;

        // --- اعمال فرمول جدید روی مسیر عکس کاور ---
        QString relativeCover = mutableBook["coverImagePath"].toString();
        if (!relativeCover.isEmpty()) {
            mutableBook["coverImagePath"] = storagePath + relativeCover;
        }

        // --- اعمال فرمول روی مسیر فایل کتاب ---
        QString relativePdf = mutableBook["pdfPath"].toString();
        if (!relativePdf.isEmpty()) {
            mutableBook["pdfPath"] = storagePath + relativePdf;
        }

        finalArray.append(mutableBook);
    }

    resp["status"] = "SUCCESS";
    resp["books"] = finalArray;
    sendJson(socket, resp);
}

//+++++کتاب های ذخیره شده+++++
void NetworkWorker::handleSaveBook(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    int bookId = data["book_id"].toInt();

    bool ok = m_dbManager->saveBook(userId, bookId);

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

    bool ok = m_dbManager->removeSavedBook(userId, bookId);

    QJsonObject resp;
    resp["action"] = "REMOVE_SAVED_BOOK_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".کتاب از لیست ذخیره شده حذف شد"
                         : ".خطا در حذف کتاب";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetSavedBooks(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    QList<QJsonObject> list = m_dbManager->getSavedBooks(userId);
    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString storagePath = QDir::cleanPath(homeDir + "/BookClub_Storage") + "/";

    for (const auto &item : list) {
        // ایجاد کپی برای تغییر مسیر کاور
        QJsonObject b = item;

        // دریافت نام فایل از دیتابیس و تبدیل به مسیر کامل
        QString coverName = b.value("coverImagePath").toString();

        if (!coverName.isEmpty()) {
            b["coverImagePath"] = storagePath + coverName;
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
    QString name = data["name"].toString().trimmed();

    bool ok = m_dbManager->createShelf(userId, name);

    QJsonObject resp;
    resp["action"] = "CREATE_SHELF_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".قفسه با موفقیت ایجاد شد"
                         : ".قفسه ای با این نام از قبل وجود دارد یا خطا رخ داده است";

    sendJson(socket, resp);
}

void NetworkWorker::handleRenameShelf(QTcpSocket* socket, const QJsonObject& data) {
    int shelfId = data["shelf_id"].toInt();
    QString newName = data["new_name"].toString().trimmed();

    bool ok = m_dbManager->renameShelf(shelfId, newName);

    QJsonObject resp;
    resp["action"] = "RENAME_SHELF_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".نام قفسه تغییر کرد"
                         : ".این نام با یکی از قفسه های دیگر شما تداخل دارد یا قفسه یافت نشد";

    sendJson(socket, resp);
}

void NetworkWorker::handleDeleteShelf(QTcpSocket* socket, const QJsonObject& data) {
    int shelfId = data["shelf_id"].toInt();

    bool ok = m_dbManager->deleteShelf(shelfId);

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

    bool ok = m_dbManager->addBookToShelf(shelfId, bookId);

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

    bool ok = m_dbManager->moveBookBetweenShelves(fromShelf, toShelf, bookId);

    QJsonObject resp;
    resp["action"] = "MOVE_BOOK_BETWEEN_SHELVES_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".کتاب با موفقیت منتقل شد"
                         : ".کتاب از قبل در قفسه مقصد موجود بود و از قفسه فعلی حذف شد";

    sendJson(socket, resp);
}

void NetworkWorker::handleGetShelves(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    QList<QJsonObject> list = m_dbManager->getShelves(userId);

    QJsonArray arr;
    for (const QJsonObject &o : std::as_const(list)){
        arr.append(o);
    }

    QJsonObject resp;
    resp["action"] = "GET_SHELVES_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["shelves"] = arr;

    sendJson(socket, resp);
}

void NetworkWorker::handleGetShelfBooks(QTcpSocket* socket, const QJsonObject& data) {
    int shelfId = data["shelf_id"].toInt();

    QList<QJsonObject> list = m_dbManager->getBooksInShelf(shelfId);
    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString storagePath = QDir::cleanPath(homeDir + "/BookClub_Storage") + "/";

    for (const auto &item : list) {
        // ساخت کپی قابل تغییر از آیتم
        QJsonObject b = item;

        // دریافت نام فایل از دیتابیس
        QString coverName = b.value("coverImagePath").toString();

        // تبدیل به مسیر مطلق برای کلاینت
        if (!coverName.isEmpty()) {
            b["coverImagePath"] = storagePath + coverName;
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

    int page = m_dbManager->getLastReadPage(userId, bookId);

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

    bool ok = m_dbManager->updateLastReadPage(userId, bookId, page);

    QJsonObject resp;
    resp["action"] = "UPDATE_LAST_READ_PAGE_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".آخرین صفحه ذخیره شد"
                         : ".خطا در ذخیره صفحه";

    sendJson(socket, resp);
}


//************************************************پنل ناشر ( ماژول 1 )*******************************************************

void NetworkWorker::handleGetPublisherProfile(QTcpSocket* socket, const QJsonObject& data) {
    int publisherId = data["publisher_id"].toInt();

    QJsonObject profile = m_dbManager->getPublisherProfile(publisherId);
    QJsonObject resp;
    resp["action"] = "GET_PUBLISHER_PROFILE_RESPONSE";

    if (profile.isEmpty()) {
        resp["status"] = "FAILED";
        resp["message"] = ".ناشر یافت نشد یا نقش کاربر ناشر نیست";
    } else {
        resp["status"] = "SUCCESS";
        resp["profile"] = profile;
    }

    sendJson(socket, resp);
}

// مدیریت آپدیت اطلاعات ناشر
void NetworkWorker::handleUpdatePublisherProfile(QTcpSocket* socket, const QJsonObject& data) {
    const int publisherId = data.value("publisher_id").toInt();

    QJsonObject resp;
    resp["action"] = "UPDATE_PUBLISHER_PROFILE_RESPONSE";

    if (m_dbManager->updatePublisherProfile(publisherId, data)) {
        resp["status"] = "SUCCESS";
        resp["message"] = ".اطلاعات پروفایل ناشر با موفقیت به روزرسانی شد";
    } else {
        resp["status"] = "FAILED";
        resp["message"] = ".خطا در ویرایش اطلاعات. این نام کاربری، نام یا ایمیل قبلاً توسط شخص دیگری انتخاب شده یا ناشر یافت نشد";
    }

    sendJson(socket, resp);
}


//************************************************پنل ناشر ( ماژول 2 )*******************************************************

void NetworkWorker::handleAddBook(QTcpSocket* socket, const QJsonObject& data)
{
    QJsonObject resp;
    resp["action"] = "ADD_BOOK_RESPONSE";

    const QString title = data.value("title").toString();
    const QString author = data.value("author").toString();
    const QString genre = data.value("genre").toString();
    const QString description = data.value("description").toString();
    const double price = data.value("price").toDouble();
    const double discountPercent = data.value("discountPercent").toDouble();
    const int publisherId = data.value("publisher_id").toInt();

    const QString publisherPdfPath = QDir::cleanPath(data.value("publisher_pdf_path").toString());
    const QString publisherCoverPath = QDir::cleanPath(data.value("publisher_cover_path").toString());

    if (title.isEmpty() || author.isEmpty() || publisherPdfPath.isEmpty() || publisherId <= 0) {
        resp["status"] = "FAILED";
        resp["message"] = ".اطلاعات اجباری کتاب یا مسیر فایل پی دی اف ارسال نشده است";
        sendJson(socket, resp);
        return;
    }

    // استفاده از مسیر استاندارد سیستم (Home) برای جلوگیری از مشکلات دسترسی اینستالر
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QDir dir(homeDir + "/BookClub_Storage");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    QString safeTitle = title.trimmed();
    safeTitle.replace(QRegularExpression("[\\s\\\\/:*?\"<>|]"), "_");

    // فقط نام فایل ها (برای ذخیره در دیتابیس)
    QString pdfFileName = QString("%1_%2.pdf").arg(timestamp).arg(safeTitle);
    QString coverFileName = publisherCoverPath.isEmpty() ? "" : QString("%1_%2_cover.jpg").arg(timestamp).arg(safeTitle);

    // مسیرهای کامل برای عملیات کپی فایل
    QString serverPdfFilePath = dir.filePath(pdfFileName);
    QString serverCoverFilePath = publisherCoverPath.isEmpty() ? "" : dir.filePath(coverFileName);

    // کپی فایل PDF
    if (!QFile::copy(publisherPdfPath, serverPdfFilePath)) {
        resp["status"] = "FAILED";
        resp["message"] = ".امکان کپی و ثبت فایل پی دی اف روی هارد دیسک سرور وجود ندارد";
        sendJson(socket, resp);
        return;
    }

    // کپی فایل کاور
    if (!publisherCoverPath.isEmpty()) {
        if (!QFile::copy(publisherCoverPath, serverCoverFilePath)) {
            QFile::remove(serverPdfFilePath);
            resp["status"] = "FAILED";
            resp["message"] = ".امکان کپی و ثبت عکس کاور روی هارد دیسک سرور وجود ندارد";
            sendJson(socket, resp);
            return;
        }
    }

    QJsonObject bookDbData;
    bookDbData["title"] = title;
    bookDbData["author"] = author;
    bookDbData["genre"] = genre;
    bookDbData["description"] = description;
    bookDbData["price"] = price;
    bookDbData["discountPercent"] = discountPercent;
    bookDbData["pdfPath"] = pdfFileName; // ذخیره نام فایل
    bookDbData["coverImagePath"] = coverFileName; // ذخیره نام فایل
    bookDbData["publisher_id"] = publisherId;

    bool success = m_dbManager->addBook(bookDbData);

    if (success) {
        resp["status"] = "SUCCESS";
        resp["message"] = ".کتاب با موفقیت کپی و در پایگاه داده ثبت شد";

        int newBookId = m_dbManager->getLastInsertedBookId();

        if (newBookId > 0 && !genre.isEmpty()) {
            QList<InterestedUser> interestedUsers = m_dbManager->getUsersInterestedInGenre(genre);
            QString messageText = QString(".کتاب جدیدی در ژانر مورد علاقه شما (%1) با نام «%2» اضافه شد").arg(genre, title);

            for (const auto& user : interestedUsers) {
                int notifId = m_dbManager->createNotification(user.id, "", "NEW_BOOK_GENRE", messageText, newBookId);

                if (notifId > 0) {
                    QJsonObject notifObj;
                    notifObj["action"] = "NEW_NOTIFICATION_RECEIVED";
                    notifObj["id"] = notifId;
                    notifObj["type"] = "NEW_BOOK_GENRE";
                    notifObj["message"] = messageText;
                    notifObj["related_id"] = newBookId;
                    notifObj["is_read"] = 0;
                    notifObj["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

                    emit notificationTriggered(user.username, notifObj);
                }
            }
        }

    } else {
        QFile::remove(serverPdfFilePath);
        if (!coverFileName.isEmpty()) QFile::remove(serverCoverFilePath);
        resp["status"] = "FAILED";
        resp["message"] = ".خطا در ثبت اطلاعات کتاب در پایگاه داده";
    }
    sendJson(socket, resp);
}

void NetworkWorker::handleUpdateBook(QTcpSocket* socket, const QJsonObject& data) {
    const int bookId = data.value("book_id").toInt();
    QJsonObject resp;
    resp["action"] = "UPDATE_BOOK_RESPONSE";

    if (bookId <= 0) {
        resp["status"] = "FAILED";
        resp["message"] = ".شناسه کتاب نامعتبر است";
        sendJson(socket, resp);
        return;
    }

    //دریافت اطلاعات فعلی از دیتابیس (برای جایگزینی مقادیر خالی)
    QJsonObject currentBook = m_dbManager->getBookDetails(bookId);
    if (currentBook.isEmpty()) {
        resp["status"] = "FAILED";
        resp["message"] = ".کتاب یافت نشد";
        sendJson(socket, resp);
        return;
    }

    // تابع کمکی: اگر در data نبود، از currentBook استفاده کن
    auto getVal = [&](const QString &key) {
        return (data.contains(key) && !data.value(key).toString().isEmpty()) ? data.value(key) : currentBook.value(key);
    };

    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QDir dir(homeDir + "/BookClub_Storage");
    if (!dir.exists()) { dir.mkpath("."); }

    const QString newPdfPath = QDir::cleanPath(data.value("publisher_pdf_path").toString());
    const QString newCoverPath = QDir::cleanPath(data.value("publisher_cover_path").toString());

    QString currentPdfFileName = currentBook.value("pdfPath").toString();
    QString currentCoverFileName = currentBook.value("coverImagePath").toString();

    QString pdfToDelete = "";
    QString coverToDelete = "";
    QString absoluteNewPdf = "";
    QString absoluteNewCover = "";

    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    QString safeTitle = getVal("title").toString().trimmed();
    safeTitle.replace(QRegularExpression("[\\s\\\\/:*?\"<>|]"), "_");

    // --- کپی فایل جدید (در صورت ارسال) ---
    if (!newPdfPath.isEmpty()) {
        QString newPdfName = QString("%1_%2.pdf").arg(timestamp).arg(safeTitle);
        absoluteNewPdf = dir.filePath(newPdfName);
        if (QFile::copy(newPdfPath, absoluteNewPdf)) {
            pdfToDelete = dir.filePath(currentPdfFileName);
            currentPdfFileName = newPdfName;
        } else {
            resp["status"] = "FAILED";
            resp["message"] = ".امکان کپی و جایگزینی فایل پی دی اف جدید روی سرور وجود ندارد";
            sendJson(socket, resp); return;
        }
    }

    if (!newCoverPath.isEmpty()) {
        QString newCoverName = QString("%1_%2_cover.jpg").arg(timestamp).arg(safeTitle);
        absoluteNewCover = dir.filePath(newCoverName);
        if (QFile::copy(newCoverPath, absoluteNewCover)) {
            coverToDelete = dir.filePath(currentCoverFileName);
            currentCoverFileName = newCoverName;
        } else {
            if (!absoluteNewPdf.isEmpty()) QFile::remove(absoluteNewPdf);
            resp["status"] = "FAILED";
            resp["message"] = ".امکان کپی و جایگزینی عکس کاور جدید روی سرور وجود ندارد";
            sendJson(socket, resp); return;
        }
    }

    // --- آماده سازی نهایی برای دیتابیس ---
    QJsonObject updatedData;
    updatedData["title"] = getVal("title");
    updatedData["author"] = getVal("author");
    updatedData["genre"] = getVal("genre");
    updatedData["description"] = getVal("description");
    updatedData["price"] = data.contains("price") ? data.value("price").toDouble() : currentBook.value("price").toDouble();
    updatedData["discountPercent"] = data.contains("discountPercent") ? data.value("discountPercent").toDouble() : currentBook.value("discountPercent").toDouble();
    updatedData["pdfPath"] = currentPdfFileName;
    updatedData["coverImagePath"] = currentCoverFileName;

    if (m_dbManager->updateBook(bookId, updatedData)) {
        // حذف فایل های قدیمی فقط بعد از موفقیت دیتابیس
        if (!pdfToDelete.isEmpty() && QFile::exists(pdfToDelete)) QFile::remove(pdfToDelete);
        if (!coverToDelete.isEmpty() && QFile::exists(coverToDelete)) QFile::remove(coverToDelete);

        resp["status"] = "SUCCESS";
        resp["message"] = ".کتاب با موفقیت ویرایش و فایل های جدید جایگزین شدند";
    } else {
        if (!absoluteNewPdf.isEmpty()) QFile::remove(absoluteNewPdf);
        if (!absoluteNewCover.isEmpty()) QFile::remove(absoluteNewCover);
        resp["status"] = "FAILED";
        resp["message"] = ".خطا در ثبت تغییرات کتاب در پایگاه داده سرور";
    }
    sendJson(socket, resp);
}

void NetworkWorker::handleSetBookDiscount(QTcpSocket* socket, const QJsonObject& data) {
    int bookId      = data["book_id"].toInt();
    int publisherId = data["publisher_id"].toInt();
    double percent  = data["discountPercent"].toDouble();

    bool ok = m_dbManager->setBookDiscount(bookId, publisherId, percent);

    QJsonObject resp;
    resp["action"] = "SET_BOOK_DISCOUNT_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? ".تخفیف با موفقیت اعمال و محاسبه شد"
                         : ".خطا در اعمال تخفیف";

    if (ok) {
        // گرفتن اطلاعات مالی و قیمت های محاسبه شده برای برودکست
        QJsonObject financials = m_dbManager->getBookFinancialDetails(bookId);

        if (!financials.isEmpty()) {
            QJsonObject b;
            b["action"] = "BOOK_DISCOUNT_UPDATED";
            b["book_id"] = bookId;
            b["price"] = financials.value("price");
            b["discountPercent"] = financials.value("discountPercent");
            b["discountAmount"] = financials.value("discountAmount");
            b["final_price"] = financials.value("final_price");

            emit broadcastRequested(b);
        }

        //گرفتن عنوان کتاب برای متن اعلان
        QString bookTitle = m_dbManager->getBookTitle(bookId);

        // پیدا کردن کاربرانی که این کتاب را در جدول saved_books ذخیره کرده اند
        QList<InterestedUser> savedUsers = m_dbManager->getUsersWhoSavedBook(bookId);
        QString messageText = QString("!کتاب «%1» که آن را ذخیره کرده بودید، تخفیف خورد").arg(bookTitle);

        for (const auto& user : savedUsers) {
            // ثبت در جدول notifications شما
            int notifId = m_dbManager->createNotification(user.id, "", "BOOK_DISCOUNT", messageText, bookId);

            if (notifId > 0) {
                QJsonObject notifObj;
                notifObj["action"] = "NEW_NOTIFICATION_RECEIVED";
                notifObj["id"] = notifId;
                notifObj["type"] = "BOOK_DISCOUNT";
                notifObj["message"] = messageText;
                notifObj["related_id"] = bookId;
                notifObj["is_read"] = 0;
                notifObj["created_at"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

                // ارسال ریل تایم به کاربر
                emit notificationTriggered(user.username, notifObj);
            }
        }
    }

    //در نهایت پاسخ نهایی به ناشر ارسال می شود
    sendJson(socket, resp);
}

void NetworkWorker::handleSetBookActiveState(QTcpSocket* socket, const QJsonObject& data) {
    int bookId      = data["book_id"].toInt();
    int publisherId = data["publisher_id"].toInt();
    bool active     = data["active"].toBool();

    bool ok = m_dbManager->setBookActiveState(bookId, publisherId, active);

    QJsonObject resp;
    resp["action"] = "SET_BOOK_ACTIVE_STATE_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    resp["message"] = ok ? (active ? ".کتاب فعال شد"
                                   : ".کتاب غیرفعال شد")
                         : ".خطا در تغییر وضعیت کتاب";

    sendJson(socket, resp);
}

void NetworkWorker::handlePublisherDeleteBook(QTcpSocket* socket, const QJsonObject& data) {
    int bookId = data.value("book_id").toInt();
    int publisherId = data.value("publisher_id").toInt(); // این آیدی باید از سمت کلاینت (لاگین شده) بیاید

    QJsonObject resp;
    resp["action"] = "PUBLISHER_DELETE_BOOK_RESPONSE";

    // فراخوانی متد جدید
    bool ok = m_dbManager->publisherDeleteBook(bookId, publisherId);

    if (ok) {
        resp["status"] = "SUCCESS";
        resp["message"] = ".کتاب با موفقیت از سیستم حذف شد";
    } else {
        resp["status"] = "ERROR";
        resp["message"] = ".خطا در حذف کتاب؛ یا کتاب موجود نیست یا متعلق به شما نمی باشد";
    }

    sendJson(socket, resp);
}

void NetworkWorker::handleGetPublisherBooks(QTcpSocket* socket, const QJsonObject& data) {
    int publisherId = data["publisher_id"].toInt();

    //دریافت لیست اصلی از دیتابیس (شامل نام فایل های ذخیره شده)
    QList<QJsonObject> list = m_dbManager->getPublisherBooks(publisherId);

    // آماده سازی مسیر ثابت Home برای اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    QJsonArray arr;

    //حلقه برای پردازش فقط عکس ها
    for (const auto &item : list) {
        // ایجاد کپی از شیء برای تغییر دادن مسیر عکس
        QJsonObject b = item;

        // فقط مسیر عکس را کامل میکنیم (نام فایل در دیتابیس باقی میماند و مسیر مطلق برای کلاینت ساخته میشود)
        if (!b.value("coverImagePath").toString().isEmpty()) {
            b["coverImagePath"] = storagePath + b.value("coverImagePath").toString();
        }

        if (!b.value("pdfPath").toString().isEmpty()) {
            b["pdfPath"] = storagePath + b.value("pdfPath").toString();
        }

        arr.append(b);
    }

    //پاسخ به کلاینت
    QJsonObject resp;
    resp["action"] = "GET_PUBLISHER_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;
    sendJson(socket, resp);
}


//************************************************پنل ناشر ( ماژول 3 )*******************************************************

void NetworkWorker::handleGetPublisherStats(QTcpSocket* socket, const QJsonObject& data) {
    int publisherId = data["publisher_id"].toInt();

    QJsonObject stats = m_dbManager->getPublisherStats(publisherId);

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر و سازگار با همه سیستم عامل ها
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString storagePath = QDir::cleanPath(homeDir + "/BookClub_Storage") + "/";

    if (stats.contains("bestSellers") && stats["bestSellers"].isArray()) {
        QJsonArray oldArr = stats["bestSellers"].toArray();
        QJsonArray newArr;
        for (const auto &val : oldArr) {
            QJsonObject obj = val.toObject();
            QString coverName = obj.value("coverImagePath").toString();
            if (!coverName.isEmpty()) {
                obj["coverImagePath"] = storagePath + coverName;
            }
            newArr.append(obj);
        }
        stats["bestSellers"] = newArr;
    }

    if (stats.contains("worstSellers") && stats["worstSellers"].isArray()) {
        QJsonArray oldArr = stats["worstSellers"].toArray();
        QJsonArray newArr;
        for (const auto &val : oldArr) {
            QJsonObject obj = val.toObject();
            QString coverName = obj.value("coverImagePath").toString();
            if (!coverName.isEmpty()) {
                obj["coverImagePath"] = storagePath + coverName;
            }
            newArr.append(obj);
        }
        stats["worstSellers"] = newArr;
    }

    QJsonObject resp;
    resp["action"] = "GET_PUBLISHER_STATS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["stats"]  = stats;

    sendJson(socket, resp);
}


//*********************************************پنل مدیر سیستم ( ماژول 1 )****************************************************

void NetworkWorker::handleGetAllUsers(QTcpSocket* socket, const QJsonObject& data) {
    Q_UNUSED(data);

    QList<QJsonObject> list = m_dbManager->getAllUsers();
    QJsonArray arr;
    for (const QJsonObject& u : std::as_const(list))
        arr.append(u);

    QJsonObject resp;
    resp["action"] = "GET_ALL_USERS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["users"]  = arr;

    sendJson(socket, resp);
}

void NetworkWorker::handleGetUserDetails(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();

    QJsonObject u = m_dbManager->getUserById(userId);

    QJsonObject resp;
    resp["action"] = "GET_USER_DETAILS_RESPONSE";

    if (u.isEmpty()) {
        resp["status"]  = "ERROR";
        resp["message"] = ".کاربر یافت نشد";
    } else {
        resp["status"]  = "SUCCESS";
        resp["user"]    = u;
    }

    sendJson(socket, resp);
}

void NetworkWorker::handleSearchUsers(QTcpSocket* socket, const QJsonObject& data) {
    QString keyword      = data["keyword"].toString();
    QString roleFilter   = data["role"].toString();
    int blockedFilter    = data["blocked"].toInt(); // کلاینت اگر فیلتر نخواهد، باید ۱- بفرستد
    QString registerDate = data["register_date"].toString(); // کلاینت اگر فیلتر نخواهد، رشته خالی "" میفرستد

    QList<QJsonObject> list = m_dbManager->searchUsers(keyword, roleFilter, blockedFilter, registerDate);
    QJsonArray arr;
    for (const QJsonObject& u : std::as_const(list))
        arr.append(u);

    QJsonObject resp;
    resp["action"] = "SEARCH_USERS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["users"]  = arr;

    sendJson(socket, resp);
}


//*********************************************پنل مدیر سیستم ( ماژول 2 )****************************************************

void NetworkWorker::handleDeleteUser(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    bool ok = m_dbManager->deleteUser(userId);

    QJsonObject resp;
    resp["action"] = "DELETE_USER_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? ".حساب کاربری حذف شد"
                         : ".خطا در حذف حساب";

    sendJson(socket, resp);
}

void NetworkWorker::handleSetUserActiveState(QTcpSocket* socket, const QJsonObject& data) {
    int userId = data["user_id"].toInt();
    bool active = data["active"].toBool();

    bool ok = m_dbManager->setUserActiveState(userId, active);

    QJsonObject resp;
    resp["action"] = "SET_USER_ACTIVE_RESPONSE";
    resp["status"] = ok ? "SUCCESS"
                        : "ERROR";
    resp["message"] = ok ? (active ? ".کاربر فعال شد"
                                   : ".کاربر غیرفعال شد")
                         : ".خطا! کاربر یافت نشد یا حساب کاربری او قبلاً حذف شده است و قابل تغییر نیست";

    sendJson(socket, resp);

}


//*********************************************پنل مدیر سیستم ( ماژول 3 )****************************************************

void NetworkWorker::handleGetAllBooks(QTcpSocket* socket, const QJsonObject& data) {
    Q_UNUSED(data);
    QList<QJsonObject> list = m_dbManager->getAllBooks();
    QJsonArray arr;

    // استفاده از مسیر استاندارد Home برای هماهنگی با اینستالر
    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";

    for (const auto &b : list) {
        QJsonObject mutableBook = b;

        QString relativeCover = mutableBook["coverImagePath"].toString();
        if (!relativeCover.isEmpty()) {
            mutableBook["coverImagePath"] = storagePath + relativeCover; // ساخت آدرس کامل برای لود عکس در ویترین
        }

        QString relativePdf = mutableBook["pdfPath"].toString();
        if (!relativePdf.isEmpty()) {
            mutableBook["pdfPath"] = storagePath + relativePdf;
        }

        arr.append(mutableBook);
    }

    QJsonObject resp;
    resp["action"] = "GET_ALL_BOOKS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["books"] = arr;

    sendJson(socket, resp);
}

void NetworkWorker::handleAdminUpdateBook(QTcpSocket* socket, const QJsonObject& data) {
    const int bookId = data.value("book_id").toInt();
    QJsonObject resp;
    resp["action"] = "ADMIN_UPDATE_BOOK_RESPONSE";

    if (bookId <= 0) {
        resp["status"] = "ERROR";
        resp["message"] = ".شناسه کتاب نامعتبر است";
        sendJson(socket, resp);
        return;
    }

    // دریافت اطلاعات فعلی از دیتابیس (بسیار مهم برای حفظ مقادیر ارسالیِ خالی)
    QJsonObject currentBook = m_dbManager->getBookDetails(bookId);
    if (currentBook.isEmpty()) {
        resp["status"] = "ERROR";
        resp["message"] = ".کتاب یافت نشد";
        sendJson(socket, resp);
        return;
    }

    // تابع کمکی برای ادغام داده‌های جدید با قدیمی
    auto getVal = [&](const QString &key) {
        return (data.contains(key) && !data.value(key).toString().isEmpty()) ? data.value(key) : currentBook.value(key);
    };

    QString storagePath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/BookClub_Storage") + "/";
    QDir dir(storagePath);
    if (!dir.exists()) { dir.mkpath("."); }

    const QString newAdminPdfPath = QDir::cleanPath(data.value("publisher_pdf_path").toString());
    const QString newAdminCoverPath = QDir::cleanPath(data.value("publisher_cover_path").toString());

    // استفاده از مقادیر فعلی دیتابیس برای مسیرهای فایل
    QString currentPdfName = currentBook.value("pdfPath").toString();
    QString currentCoverName = currentBook.value("coverImagePath").toString();

    QString pdfToDelete = "";
    QString coverToDelete = "";
    QString absoluteNewPdf = "";
    QString absoluteNewCover = "";

    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    QString safeTitle = getVal("title").toString().trimmed();
    safeTitle.replace(QRegularExpression("[\\s\\\\/:*?\"<>|]"), "_");

    // --- جایگزینی فیزیکی فایل PDF ---
    if (!newAdminPdfPath.isEmpty()) {
        QString newFileName = QString("%1_%2.pdf").arg(timestamp).arg(safeTitle);
        absoluteNewPdf = storagePath + newFileName;

        if (QFile::copy(newAdminPdfPath, absoluteNewPdf)) {
            if (!currentPdfName.isEmpty()) { pdfToDelete = storagePath + currentPdfName; }
            currentPdfName = newFileName;
        } else {
            resp["status"] = "ERROR";
            resp["message"] = ".امکان کپی و جایگزینی فایل پی دی اف جدید روی سرور وجود ندارد";
            sendJson(socket, resp); return;
        }
    }

    // --- جایگزینی فیزیکی عکس کاور ---
    if (!newAdminCoverPath.isEmpty()) {
        QString newCoverName = QString("%1_%2_cover.jpg").arg(timestamp).arg(safeTitle);
        absoluteNewCover = storagePath + newCoverName;

        if (QFile::copy(newAdminCoverPath, absoluteNewCover)) {
            if (!currentCoverName.isEmpty()) { coverToDelete = storagePath + currentCoverName; }
            currentCoverName = newCoverName;
        } else {
            if (!absoluteNewPdf.isEmpty()) QFile::remove(absoluteNewPdf);
            resp["status"] = "ERROR";
            resp["message"] = ".امکان کپی و جایگزینی عکس کاور جدید روی سرور وجود ندارد";
            sendJson(socket, resp); return;
        }
    }

    // آماده سازی داده های نهایی برای دیتابیس
    QJsonObject updatedData;
    updatedData["title"] = getVal("title");
    updatedData["author"] = getVal("author");
    updatedData["genre"] = getVal("genre");
    updatedData["description"] = getVal("description");
    updatedData["price"] = data.contains("price") ? data.value("price").toDouble() : currentBook.value("price").toDouble();
    updatedData["discountPercent"] = data.contains("discountPercent") ? data.value("discountPercent").toDouble() : currentBook.value("discountPercent").toDouble();
    updatedData["pdfPath"] = currentPdfName;
    updatedData["coverImagePath"] = currentCoverName;

    if (m_dbManager->adminUpdateBook(bookId, updatedData)) {
        if (!pdfToDelete.isEmpty() && QFile::exists(pdfToDelete)) QFile::remove(pdfToDelete);
        if (!coverToDelete.isEmpty() && QFile::exists(coverToDelete)) QFile::remove(coverToDelete);

        resp["status"] = "SUCCESS";
        resp["message"] = ".اطلاعات و فایل های کتاب با موفقیت توسط ادمین ویرایش شد";
    } else {
        if (!absoluteNewPdf.isEmpty()) QFile::remove(absoluteNewPdf);
        if (!absoluteNewCover.isEmpty()) QFile::remove(absoluteNewCover);
        resp["status"] = "ERROR";
        resp["message"] = ".خطا در ویرایش کتاب در دیتابیس";
    }

    sendJson(socket, resp);
}

void NetworkWorker::handleAdminDeleteBook(QTcpSocket* socket, const QJsonObject& data) {
    int bookId = data["book_id"].toInt();

    // این متد اکنون در دیتابیس هم کتاب را غیرفعال میکند و هم پرچم حذف را ۱ میزند
    bool ok = m_dbManager->adminDeleteBook(bookId);

    QJsonObject resp;
    resp["action"] = "ADMIN_DELETE_BOOK_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";

    resp["message"] = ok ? ".کتاب با موفقیت از چرخه سیستم حذف منطقی و غیرفعال شد"
                         : ".خطا در حذف منطقی کتاب از پایگاه داده";

    sendJson(socket, resp);
}


//****************************************************سیستم اعلان ها**********************************************************

void NetworkWorker::handleGetNotifications(QTcpSocket* socket, const QJsonObject& data) {
    // استخراج مقادیر متناسب با ورودی های تابع دیتابیس
    const int userId = data.value("user_id").toInt();
    const QString role = data.value("role").toString();

    QList<QJsonObject> list = m_dbManager->getNotifications(userId, role);

    QJsonArray arr;
    for (const QJsonObject& n : std::as_const(list))
        arr.append(n);

    QJsonObject resp;
    resp["action"] = "GET_NOTIFICATIONS_RESPONSE";
    resp["status"] = "SUCCESS";
    resp["notifications"] = arr;
    sendJson(socket, resp);
}

void NetworkWorker::handleMarkNotificationRead(QTcpSocket* socket, const QJsonObject& data) {
    const int id = data.value("notification_id").toInt();
    const bool ok = m_dbManager->markNotificationRead(id);

    QJsonObject resp;
    resp["action"] = "MARK_NOTIFICATION_READ_RESPONSE";
    resp["status"] = ok ? "SUCCESS" : "ERROR";
    sendJson(socket, resp);
}



