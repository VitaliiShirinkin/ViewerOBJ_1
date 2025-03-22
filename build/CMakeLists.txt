cmake_minimum_required(VERSION 3.10)
project(CombineFiles)

# Указываем путь к исходным файлам
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/..)

# Указываем путь к скрипту
set(SCRIPT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/combine_files.py)

# Указываем путь к выходному файлу
set(OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/combined_code.cpp)

# Добавляем пользовательскую команду для выполнения скрипта
add_custom_command(
    OUTPUT ${OUTPUT_FILE}
    COMMAND python ${SCRIPT_PATH}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Объединение файлов .cpp и .h в один файл"
)

# Добавляем цель для выполнения команды
add_custom_target(CombineFiles ALL DEPENDS ${OUTPUT_FILE})
