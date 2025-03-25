#include "viewer.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>

// Конструктор класса Viewer
Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0), selectedVertexIndex(-1)
{
    setMinimumSize(400, 400); // Устанавливаем минимальный размер окна
    setMouseTracking(true); // Включаем отслеживание движения мыши
}

// Метод для установки модели
void Viewer::setModel(Model *model) {
    this->model = model; // Устанавливаем модель
    update(); // Обновляем отображение
}

// Метод для установки масштаба
void Viewer::setScale(float scale) {
    this->scale = scale; // Устанавливаем масштаб
    update(); // Обновляем отображение
}

// Метод для обработки событий рисования
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

    // Отрисовка осей координат
    int margin = 20; // Отступ от краев экрана
    int axisLength = qMax(width(), height()); // Длина осей равна максимальному размеру окна

    // Ось X (красная)
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(margin, height() - margin, width() - margin, height() - margin); // Ось X
    painter.drawText(width() - margin - 10, height() - margin + 15, "X");

    // Ось Y (зеленая)
    painter.setPen(QPen(Qt::green, 2));
    painter.drawLine(margin, height() - margin, margin, margin); // Ось Y
    painter.drawText(margin - 10, margin + 15, "Y");
    // Ось Z (синяя)
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(margin, height() - margin, margin + axisLength / 2, height() - margin - axisLength / 2); // Ось Z
    painter.drawText(margin + axisLength / 2 + 10, height() - margin - axisLength / 2 - 10, "Z");

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
            // Выделяем выбранную вершину красным цветом
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

    // Отображаем координаты и индекс выбранной вершины
    if (selectedVertexIndex != -1) {
        const QVector3D &vertex = vertices[selectedVertexIndex];
        QString info = QString("Вершина [%1]: (%2, %3, %4)")
                          .arg(selectedVertexIndex) // Индекс вершины
                          .arg(vertex.x(), 0, 'f', 2) // Координата X
                          .arg(vertex.y(), 0, 'f', 2) // Координата Y
                          .arg(vertex.z(), 0, 'f', 2); // Координата Z
        painter.setPen(QPen(Qt::black, 2));
        painter.drawText(10, 20, info); // Отображаем информацию в верхнем левом углу
    }
}

// Метод для обработки нажатий кнопок мыши
void Viewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos(); // Запоминаем позицию мыши

        if (model) {
            const QVector<QVector3D> &vertices = model->getVertices();
            int closestVertexIndex = -1;
            float minDistance = 10.0f; // Максима
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
                update(); // Обновляем отображение
            }
        }
    }
}

// Метод для обработки перемещения мыши
void Viewer::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // Вычисляем изменение углов поворота
        rotationX += (event->y() - lastMousePosition.y()) * 0.01;
        rotationY += (event->x() - lastMousePosition.x()) * 0.01;
        lastMousePosition = event->pos(); // Обновляем позицию мыши
        update(); // Обновляем отображение
    }
}

// Метод для обработки событий прокрутки колесика мыши
void Viewer::wheelEvent(QWheelEvent *event) {
    // Масштабирование модели
    if (event->angleDelta().y() > 0) {
        scale *= 1.1; // Увеличение масштаба
    } else {
        scale /= 1.1; // Уменьшение масштаба
    }
    update(); // Обновляем отображение
}
