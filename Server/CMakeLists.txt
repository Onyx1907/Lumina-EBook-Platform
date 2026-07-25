cmake_minimum_required(VERSION 3.16)

# ۱. تعریف نام پروژه سرور
project(LuminaServer VERSION 0.1 LANGUAGES CXX)

# فعال کردن ویژگی‌های خودکار Qt برای فرم گرافیکی پایش سرور
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ۲. پیدا کردن پکیج‌های مورد نیاز سرور (هسته، ویدجت‌ها برای UI مانیتورینگ و شبکه برای سوکت)
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)


# ۳. فایل‌های سورس سرور (دوستت هر فایلی ساخت، اینجا اضافه میشه)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# ۴. ساخت فایل اجرایی سرور با نام LuminaServer
qt_add_executable(LuminaServer
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    database_manager.h database_manager.cpp
    server.h
    server.cpp
    networkworker.h networkworker.cpp
)

# ۵. جادوی اصلی: لینک کردن لایبرری مشترک (توابع هش و کلاس‌ها) و ابزارهای کیوت به سرور
target_link_libraries(LuminaServer PRIVATE
    LuminaShared
    Qt6::Widgets
    Qt6::Sql
    Qt6::Core
    Qt6::Network
)

# تنظیمات پیش‌فرض برای خروجی سیستم‌عامل‌ها
set_target_properties(LuminaServer PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LuminaServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(LuminaServer)
