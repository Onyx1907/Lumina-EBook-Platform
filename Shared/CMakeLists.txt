# Shared/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# ساخت لایبرری استاتیک
add_library(LuminaShared STATIC
    constants.h
    user.h
    admin.h
    book.h
    publisher.h
    regularuser.h
    user.cpp
    admin.cpp
    book.cpp
    publisher.cpp
    regularuser.cpp
)

# معرفی مسیر هدرها به پروژه اصلی و بقیه زیرپروژه‌ها
target_include_directories(LuminaShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# چون کلاس‌های شما (مثل یوزر و کتاب) احتمالاً از QString یا کامپوننت‌های Qt استفاده می‌کنند،
# حتماً باید هسته کیوت به خود لایبرری هم لینک بشه:
find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(LuminaShared PUBLIC Qt6::Core)
