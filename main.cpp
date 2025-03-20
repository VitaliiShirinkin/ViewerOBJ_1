#include <QApplication>  // Основной класс приложения Qt
#include "mainwindow.h"  // Заголовочный файл главного окна

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // Создание объекта приложения
    MainWindow window;             // Создание объекта главного окна
    window.show();                 // Отображение главного окна
    return app.exec();             // Запуск основного цикла приложения
}
