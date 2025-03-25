// Подключаем основной класс приложения Qt
#include <QApplication>
// Подключаем заголовочный файл главного окна
#include "mainwindow.h"

// Точка входа в программу
int main(int argc, char *argv[])
{
    // Создаем объект приложения
    QApplication app(argc, argv);
    // Создаем объект главного окна
    MainWindow window;
    // Отображаем главное окно
    window.show();
    // Запускаем основной цикл приложения
    return app.exec();
}
