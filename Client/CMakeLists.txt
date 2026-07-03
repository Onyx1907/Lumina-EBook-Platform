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
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        loginwidget.h  loginwidget.cpp  loginwidget.ui
        registerwidget.h  registerwidget.cpp  registerwidget.ui
        resources.qrc
)

# ۳. نام تارگت رو از Client به LuminaClient تغییر دادم
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


)

# ۴. اینجا لومینا شیرد و کیوت ویدجت و شبکه رو به لومینا کلاینت وصل می‌کنیم
target_link_libraries(LuminaClient PRIVATE
    LuminaShared
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

set_target_properties(LuminaClient PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(LuminaClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/user)

include(GNUInstallDirs)
install(TARGETS LuminaClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(LuminaClient)
