# Минимальная версия CMake
cmake_minimum_required(VERSION 3.14)

# Название проекта
project(My3DModelViewer)

# Установка стандарта C++17
set(CMAKE_CXX_STANDARD 17)

# Включение автоматической генерации MOC, RCC и UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Поиск и подключение необходимых модулей Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Список исходных файлов
set(SOURCES
    main.cpp
    model.cpp
    viewer.cpp
    modelviewer.cpp
    mainwindow.cpp
)

# Список заголовочных файлов
set(HEADERS
    model.h
    viewer.h
    modelviewer.h
    mainwindow.h
)

# Создание исполняемого файла с указанием исходных и заголовочных файлов
add_executable(My3DModelViewer
    ${SOURCES}
    ${HEADERS}
)

# Подключение библиотек Qt6 к проекту
target_link_libraries(My3DModelViewer PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
