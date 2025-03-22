#include "viewer.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0)
{
    setMinimumSize(400, 400); // Устанавливаем минимальный размер окна
    setMouseTracking(true); // Включаем отслеживание движения мыши
}

void Viewer::setModel(Model *model) {
    this->model = model; // Устанавливаем модель
    update(); // Обновляем отображение
}

void Viewer::setScale(float scale) {
    this->scale = scale; // Устанавливаем масштаб
    update(); // Обновляем отображение
}

void Viewer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // Включаем сглаживание

    if (!model) {
        // Если модель не загружена, выводим сообщение
        painter.drawText(rect(), Qt::AlignCenter, "Модель не загружена");
        return;
    }

    // Очищаем экран белым цветом
    painter.fillRect(rect(), Qt::white);

    // Получаем вершины и грани модели
    const QVector<QVector3D> &vertices = model->getVertices();
    const QVector<QVector<int>> &faces = model->getFaces();

    // Отрисовываем вершины
    painter.setPen(QPen(Qt::black, 2));
    for (const QVector3D &vertex : vertices) {
        // Применяем поворот и масштабирование
        float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
        float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
        float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
        z = vertex.y() * sin(rotationX) + z * cos(rotationX);

        // Преобразуем координаты в экранные
        QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
        painter.drawEllipse(point, 3, 3); // Рисуем вершину
    }

    // Отрисовываем грани
    painter.setPen(QPen(Qt::blue, 2));
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue; // Пропускаем неполные грани
        QPolygonF polygon;
        for (int index : face) {
            const QVector3D &vertex = vertices[index];
            // Применяем поворот и масштабирование
            float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
            float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
            float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
            z = vertex.y() * sin(rotationX) + z * cos(rotationX);

            // Преобразуем координаты в экранные
            QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
            polygon << point; // Добавляем точку в полигон
        }
        painter.drawPolygon(polygon); // Рисуем грань
    }
}

void Viewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos(); // Запоминаем позицию мыши
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // Вычисляем изменение углов поворота
        rotationX += (event->y() - lastMousePosition.y()) * 0.01;
        rotationY += (event->x() - lastMousePosition.x()) * 0.01;
        lastMousePosition = event->pos(); // Обновляем позицию мыши
        update(); // Обновляем отображение
    }
}

void Viewer::wheelEvent(QWheelEvent *event) {
    // Масштабирование модели
    if (event->angleDelta().y() > 0) {
        scale *= 1.1; // Увеличение масштаба
    } else {
        scale /= 1.1; // Уменьшение масштаба
    }
    update(); // Обновляем отображение
}

