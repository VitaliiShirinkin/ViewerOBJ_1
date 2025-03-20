#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>          // Базовый класс для всех виджетов в Qt
#include "model.h"          // Подключение заголовочного файла модели

class Viewer : public QWidget
{
    Q_OBJECT  // Макрос для поддержки сигналов и слотов

public:
    Viewer(QWidget *parent = nullptr);  // Конструктор класса Viewer
    void setModel(Model *model);         // Метод для установки модели
    void paintEvent(QPaintEvent *event) override;  // Переопределение метода отрисовки

protected:
    void mousePressEvent(QMouseEvent *event) override;  // Обработка нажатий мыши
    void mouseMoveEvent(QMouseEvent *event) override;   // Обработка движения мыши
    void wheelEvent(QWheelEvent *event) override;       // Обработка прокрутки колеса мыши

private:
    Model *model;  // Указатель на объект модели
    QPoint lastMousePosition;  // Последняя позиция мыши для вращения модели
    float rotationX;  // Угол вращения вокруг оси X
    float rotationY;  // Угол вращения вокруг оси Y
};

#endif // VIEWER_H
