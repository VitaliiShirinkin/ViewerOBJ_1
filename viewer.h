#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget> // Подключаем основной класс для создания виджетов
#include "model.h" // Подключаем файл модели, который будет использоваться в Viewer

// Объявляем класс Viewer, который наследует от QWidget
class Viewer : public QWidget
{
    Q_OBJECT // Макрос, необходимый для использования сигналов и слотов в Qt

public:
    // Конструктор класса Viewer, принимает родительский виджет
    explicit Viewer(QWidget *parent = nullptr);

    // Метод для установки модели, с которой будет работать Viewer
    void setModel(Model *model);

    // Метод для установки масштаба отображаемой модели
    void setScale(float scale);

protected:
    // Переопределяем метод для обработки событий рисования
    void paintEvent(QPaintEvent *event) override;

    // Переопределяем метод для обработки нажатий кнопок мыши
    void mousePressEvent(QMouseEvent *event) override;

    // Переопределяем метод для обработки перемещения мыши
    void mouseMoveEvent(QMouseEvent *event) override;

    // Переопределяем метод для обработки событий прокрутки колесика мыши
    void wheelEvent(QWheelEvent *event) override;

private:
    Model *model; // Указатель на объект модели, который будет отображаться
    QPoint lastMousePosition; // Хранит последнюю позицию курсора мыши
    float rotationX; // Угол поворота модели по оси X
    float rotationY; // Угол поворота модели по оси Y
    float scale; // Масштаб модели для отображения
    int selectedVertexIndex; // Индекс выбранной вершины модели (если есть)
};

#endif // VIEWER_H // Завершаем условную компиляцию
