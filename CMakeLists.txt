# ./CMakeLists.txt (ریشه اصلی)
cmake_minimum_required(VERSION 3.16)
project(LuminaProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# پیدا کردن پکیج‌های کیوت در بالاترین سطح
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# اضافه کردن پوشه‌ها به ترتیب درست
add_subdirectory(Shared)
add_subdirectory(Client)
add_subdirectory(Server) 
