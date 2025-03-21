#include "viewer.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFontMetrics>

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0)
{
    setMinimumSize(400, 400);
}

void Viewer::setModel(Model *model)
{
    this->model = model;
    update();
}

void Viewer::setScale(float scale)
{
    this->scale = scale;
    update();
}

void Viewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!model) {
        painter.drawText(rect(), Qt::AlignCenter, "Модель не загружена");
        return;
    }

    // Отрисовка модели
    const QVector<QVector3D> &vertices = model->getVertices();
    const QVector<QVector<int>> &faces = model->getFaces();

    painter.setPen(QPen(Qt::black, 2));
    for (const QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
        float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
        float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
        z = vertex.y() * sin(rotationX) + z * cos(rotationX);

        QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
        painter.drawEllipse(point, 3, 3);
    }

    painter.setPen(QPen(Qt::blue, 2));
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue;
        QPolygonF polygon;
        for (int index : face) {
            const QVector3D &vertex = vertices[index];
            float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
            float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
            float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
            z = vertex.y() * sin(rotationX) + z * cos(rotationX);

            QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
            polygon << point;
        }
        painter.drawPolygon(polygon);
    }

    // Вывод информации о модели в столбик
    QVector3D dimensions = model->getModelDimensions();
    double volume = model->calculateVolume();
    double area = model->calculateProjectionArea();

    // Форматирование текста
    QStringList infoLines;
    infoLines << "Размеры модели:";
    infoLines << QString("X: %1 м").arg(dimensions.x(), 0, 'f', 2);
    infoLines << QString("Y: %1 м").arg(dimensions.y(), 0, 'f', 2);
    infoLines << QString("Z: %1 м").arg(dimensions.z(), 0, 'f', 2);
    infoLines << ""; // Пустая строка для разделения
    infoLines << QString("Объем: %1 м³").arg(volume, 0, 'f', 2);
    infoLines << ""; // Пустая строка для разделения
    infoLines << QString("Площадь проекции: %1 м²").arg(area, 0, 'f', 2);

    // Установка шрифта и цвета
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10));

    // Вывод текста в столбик
    int lineHeight = painter.fontMetrics().height();
    int yOffset = 20; // Начальная позиция по вертикали
    for (const QString &line : infoLines) {
        painter.drawText(10, yOffset,
                         line); // Вывод каждой строки
                                yOffset += lineHeight; // Смещение вниз на высоту строки
                            }
                        }

                        void Viewer::mousePressEvent(QMouseEvent *event)
                        {
                            lastMousePosition = event->pos();
                        }

                        void Viewer::mouseMoveEvent(QMouseEvent *event)
                        {
                            if (event->buttons() & Qt::LeftButton) {
                                rotationX += (event->y() - lastMousePosition.y()) * 0.5;
                                rotationY += (event->x() - lastMousePosition.x()) * 0.5;
                                lastMousePosition = event->pos();
                                update();
                            }
                        }

                        void Viewer::wheelEvent(QWheelEvent *event)
                        {
                            // Масштабирование модели
                            if (event->angleDelta().y() > 0) {
                                scale *= 1.1; // Увеличение масштаба
                            } else {
                                scale /= 1.1; // Уменьшение масштаба
                            }
                            update();
                        }
