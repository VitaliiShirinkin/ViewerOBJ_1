#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> // Основной класс для создания главного окна
#include "modelviewer.h" // Подключение класса ModelViewer для работы с 3D-моделью

class MainWindow : public QMainWindow
{
    Q_OBJECT // Макрос для поддержки сигналов и слотов

public:
    // Конструктор главного окна
    MainWindow(QWidget *parent = nullptr);

    // Деструктор
    ~MainWindow();

private slots:
    // Слот для открытия модели
    void openModel();

    // Слот для сохранения текста (заголовка окна) в файл
    void saveText();

private:
    // Указатель на виджет для отображения и работы с 3D-моделью
    ModelViewer *modelViewer;
};

#endif // MAINWINDOW_H
