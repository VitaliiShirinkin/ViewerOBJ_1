#include "viewer.h"          // Подключение заголовочного файла Viewer
#include <QPainter>          // Для работы с графикой
#include <QPen>              // Для настройки пера (цвета и стиля линий)
#include <QMouseEvent>       // Для обработки событий мыши
#include <QWheelEvent>       // Для обработки событий прокрутки колеса мыши

Viewer::Viewer(QWidget *parent) 
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0)  // Инициализация базового класса и указателей
{
    setMinimumSize(400, 400);  // Установка минимального размера виджета
}

// Метод для установки модели, которую нужно отобразить
void Viewer::setModel(Model *model)
{
    this->model = model;  // Сохранение указателя на модель
    update();             // Запрос перерисовки виджета
}

// Метод для обработки события отрисовки
void Viewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);  // Убираем предупреждение о неиспользуемом параметре

    QPainter painter(this);  // Создание объекта рисовальщика
    painter.setRenderHint(QPainter::Antialiasing);  // Включение сглаживания для более качественной отрисовки

    // Проверка, установлена ли модель
    if (!model) {
        painter.drawText(rect(), Qt::AlignCenter, "No Model Loaded");  // Если модель не загружена, выводим сообщение
        return;  // Завершаем выполнение метода
    }

    // Получение данных о вершинах и гранях модели через публичные методы
    const QVector<QVector3D> &vertices = model->getVertices();  // Получение списка вершин
    const QVector<QVector<int>> &faces = model->getFaces();     // Получение списка граней

    // Отрисовка вершин модели
    painter.setPen(QPen(Qt::black, 2));  // Настройка пера для рисования (черный цвет, толщина 2)
    for (const QVector3D &vertex : vertices) {
        // Преобразование 3D координат в 2D для отображения с учетом вращения
        float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
        float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
        float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
        z = vertex.y() * sin(rotationX) + z * cos(rotationX);

        // Центрируем модель
        QPointF point(x + width() / 2, height() / 2 - y);
        painter.drawEllipse(point, 3, 3);  // Отрисовка вершины как круга
    }

    // Отрисовка граней модели
    painter.setPen(QPen(Qt::blue, 2));  // Настройка пера для рисования (синий цвет, толщина 2)
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue;  // Пропускаем грани с менее чем 3 вершинами
        QPolygonF polygon;  // Создание многоугольника для грани
        for (int index : face) {
            const QVector3D &vertex = vertices[index];  // Получение вершины по индексу
            // Преобразование 3D координат в 2D для отображения с учетом вращения
            float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
            float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
            float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
            z = vertex.y() * sin(rotationX) + z * cos(rotationX);

            // Центрируем модель
            QPointF point(x + width() / 2, height() / 2 - y);
            polygon << point;  // Добавление точки в многоугольник
        }
        painter.drawPolygon(polygon);  // Отрисовка грани

    }
}

// Обработка нажатий мыши для начала вращения модели
void Viewer::mousePressEvent(QMouseEvent *event)
{
    lastMousePosition = event->pos();  // Сохранение позиции мыши
}

// Обработка движения мыши для вращения модели
void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {  // Если нажата левая кнопка мыши
        // Вычисление изменения углов вращения
        rotationX += (event->y() - lastMousePosition.y()) * 0.5;  // Изменение угла вращения по оси X
        rotationY += (event->x() - lastMousePosition.x()) * 0.5;  // Изменение угла вращения по оси Y
        lastMousePosition = event->pos();  // Обновление последней позиции мыши
        update();  // Запрос перерисовки виджета
    }
}

// Обработка прокрутки колеса мыши (можно использовать для изменения масштаба)
void Viewer::wheelEvent(QWheelEvent *event)
{
    // Можно реализовать изменение масштаба модели, если это необходимо
    // Например, можно изменить значения rotationX и rotationY для создания эффекта приближения
}
