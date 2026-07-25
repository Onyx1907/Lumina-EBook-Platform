cmake_minimum_required(VERSION 3.16)

# ۱. اسم پروژه رو بذار LuminaClient که با پوشه‌ها قاطی نشه
project(LuminaClient VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ۲. پکیج Network رو هم اینجا اضافه کن برای سوکت‌ها
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets Pdf PdfWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        loginwidget.h  loginwidget.cpp  loginwidget.ui
        registerwidget.h  registerwidget.cpp  registerwidget.ui
        resources.qrc
)

qt_add_executable(LuminaClient
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    resources.qrc
    loginwidget.h loginwidget.cpp loginwidget.ui
    registerwidget.h registerwidget.cpp registerwidget.ui
    forgotpasswordwidget.h forgotpasswordwidget.cpp forgotpasswordwidget.ui
    clientnetworkmanager.h clientnetworkmanager.cpp

    userdashboardwidget.h userdashboardwidget.cpp userdashboardwidget.ui
    user/genreselectionwidget.h user/genreselectionwidget.cpp user/genreselectionwidget.ui
    user/userhomewidget.h user/userhomewidget.cpp user/userhomewidget.ui
    user/bookcard.h user/bookcard.cpp user/bookcard.ui
    user/profilewidget.h user/profilewidget.cpp user/profilewidget.ui
    user/bookdetailswidget.h user/bookdetailswidget.cpp user/bookdetailswidget.ui
    user/searchwidget.h user/searchwidget.cpp user/searchwidget.ui
    user/bookinfocard.h user/bookinfocard.cpp user/bookinfocard.ui
    user/resultwidget.h user/resultwidget.cpp user/resultwidget.ui
    user/commentitemwidget.h user/commentitemwidget.cpp user/commentitemwidget.ui
    user/commentswidget.h user/commentswidget.cpp user/commentswidget.ui
    user/cartwidget.h user/cartwidget.cpp user/cartwidget.ui
    user/librarywidget.h user/librarywidget.cpp user/librarywidget.ui
    storageutils.h storageutils.cpp
    user/bookreaderwidget.h user/bookreaderwidget.cpp user/bookreaderwidget.ui
    publisherdashboardwidget.h publisherdashboardwidget.cpp publisherdashboardwidget.ui
    publisher/publisherstatswidget.h publisher/publisherstatswidget.cpp publisher/publisherstatswidget.ui
    sharedbookcard.h sharedbookcard.cpp sharedbookcard.ui
    publisher/publisherbookwidget.h publisher/publisherbookwidget.cpp publisher/publisherbookwidget.ui
    publisher/editbookwidget.h publisher/editbookwidget.cpp publisher/editbookwidget.ui
    user/purchasehistorywidget.h user/purchasehistorywidget.cpp user/purchasehistorywidget.ui
    user/inovicecard.h user/inovicecard.cpp user/inovicecard.ui
    admindashboardwidget.h admindashboardwidget.cpp admindashboardwidget.ui
    admin/adminbookswidget.h admin/adminbookswidget.cpp admin/adminbookswidget.ui





)

# ۴. اینجا لومینا شیرد و کیوت ویدجت و شبکه رو به لومینا کلاینت وصل می‌کنیم
target_link_libraries(LuminaClient PRIVATE
    LuminaShared
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Pdf
    Qt6::PdfWidgets
)

set_target_properties(LuminaClient PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(LuminaClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/user
    ${CMAKE_CURRENT_SOURCE_DIR}/publisher
    ${CMAKE_CURRENT_SOURCE_DIR}/admin)

include(GNUInstallDirs)
install(TARGETS LuminaClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(LuminaClient)
