// Защита от повторного включения заголовочного файла
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Подключаем базовый класс для главного окна
#include <QMainWindow>
// Подключаем заголовочный файл для работы с моделью и отображением
#include "modelviewer.h"

// Класс главного окна
class MainWindow : public QMainWindow
{
    Q_OBJECT  // Макрос для поддержки сигналов и слотов

public:
    // Конструктор с параметром родительского виджета
    MainWindow(QWidget *parent = nullptr);
    // Деструктор
    ~MainWindow();

private slots:
    // Слот для открытия модели
    void openModel();
    // Слот для сохранения текста
    void saveText();
    // Слот для поворота модели
    void rotateModel();
    // Слот для перемещения модели
    void translateModel();
    // Слот для обновления заголовка окна
    void updateWindowTitle();

private:
    // Указатель на объект для работы с моделью и отображением
    ModelViewer *modelViewer;
};

// Завершение защиты от повторного включения
#endif // MAINWINDOW_H
