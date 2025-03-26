#include "viewer.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0), selectedVertexIndex(-1)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);
}

void Viewer::setModel(Model *model) {
    this->model = model;
    update();
}

void Viewer::setScale(float scale) {
    this->scale = scale;
    update();
}

void Viewer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!model) {
        painter.drawText(rect(), Qt::AlignCenter, "Модель не загружена");
        return;
    }

    // Очищаем экран белым цветом
    painter.fillRect(rect(), Qt::white);

    // Отрисовка осей координат (маленькие, в левом верхнем углу)
    int margin = 10; // Отступ от края
    int axisLength = 100; // Длина осей

    // Ось X (красная)
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(margin, margin, margin + axisLength, margin); // Ось X
    painter.drawText(margin + axisLength + 5, margin + 5, "X");

    // Ось Y (зеленая)
    painter.setPen(QPen(Qt::green, 2));
    painter.drawLine(margin, margin, margin, margin + axisLength); // Ось Y
    painter.drawText(margin - 5, margin + axisLength + 10, "Y");

    // Ось Z (синяя)
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(margin, margin, margin + axisLength / 2, margin + axisLength / 2); // Ось Z
    painter.drawText(margin + axisLength / 2 + 5, margin + axisLength / 2 - 5, "Z");

    // Получаем вершины и грани модели
    const QVector<QVector3D> &vertices = model->getVertices();
    const QVector<QVector<int>> &faces = model->getFaces();

    // Отрисовываем вершины
    painter.setPen(QPen(Qt::black, 2));
    for (int i = 0; i < vertices.size(); ++i) {
        const QVector3D &vertex = vertices[i];

        // Применяем поворот и масштабирование
        float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
        float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
        float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
        z = vertex.y() * sin(rotationX) + z * cos(rotationX);

        // Преобразуем координаты в экранные
        QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));

        // Отрисовываем вершину
        if (i == selectedVertexIndex) {
            painter.setPen(QPen(Qt::red, 4));
            painter.drawEllipse(point, 5, 5);
            painter.setPen(QPen(Qt::black, 2));
        } else {
            painter.drawEllipse(point, 3, 3);
        }
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
        lastMousePosition = event->pos();

        if (model) {
            const QVector<QVector3D> &vertices = model->getVertices();
            int closestVertexIndex = -1;
            float minDistance = 10.0f; // Максимальное расстояние для выбора вершины

            // Ищем ближайшую вершину к месту клика
            for (int i = 0; i < vertices.size(); ++i) {
                const QVector3D &vertex = vertices[i];
                // Применяем поворот и масштабирование
                                float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
                                float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
                                float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
                                z = vertex.y() * sin(rotationX) + z * cos(rotationX);
                                // Преобразуем координаты в экранные
                                QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));

                                // Вычисляем расстояние от курсора мыши до вершины
                                float distance = QLineF(event->pos(), point).length();
                                if (distance < minDistance) {
                                    minDistance = distance;
                                    closestVertexIndex = i;
                                }
                            }

                            // Если найдена ближайшая вершина, выбираем её
                            if (closestVertexIndex != -1) {
                                selectedVertexIndex = closestVertexIndex;

                                // Получаем координаты выделенной вершины
                                const QVector3D &selectedVertex = vertices[selectedVertexIndex];

                                // Отправляем информацию о выделенной вершине в MainWindow
                                emit vertexSelected(selectedVertexIndex, selectedVertex);

                                update(); // Обновляем отображение
                            }
                        }
                    }
                }

void Viewer::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // Используем position() для получения текущей позиции мыши
        rotationX += (event->position().y() - lastMousePosition.y()) * 0.01;
        rotationY += (event->position().x() - lastMousePosition.x()) * 0.01;
        lastMousePosition = event->position(); // Обновляем позицию мыши
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

