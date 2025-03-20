#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>      // Базовый класс для главного окна приложения
#include "modelviewer.h"     // Подключение заголовочного файла ModelViewer

class MainWindow : public QMainWindow
{
    Q_OBJECT  // Макрос для поддержки сигналов и слотов

public:
    MainWindow(QWidget *parent = nullptr);  // Конструктор класса MainWindow
    ~MainWindow();  // Деструктор класса MainWindow

private slots:
    void openModel();  // Слот для открытия модели из файла

private:
    ModelViewer *modelViewer;  // Указатель на объект ModelViewer для отображения модели
};

#endif // MAINWINDOW_H
