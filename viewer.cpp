#include "viewer.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVBoxLayout>
#include <QHBoxLayout> // Добавлен для горизонтального размещения

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0),
      targetRotationX(0), targetRotationY(0), rotationSpeed(10)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);

    // Инициализация таймера для плавного вращения
    rotationTimer = new QTimer(this);
    connect(rotationTimer, &QTimer::timeout, this, &Viewer::updateRotation);
    rotationTimer->start(16); // ~60 FPS (16 мс)

    // Добавление ползунка для регулировки скорости вращения
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1, 100); // Диапазон скорости
    speedSlider->setValue(rotationSpeed);
    speedSlider->setFixedWidth(200); // Уменьшение длины ползунка
    connect(speedSlider, &QSlider::valueChanged, this, &Viewer::setRotationSpeed);

    // Размещение ползунка внизу окна
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addStretch(); // Добавляем растяжение слева
    sliderLayout->addWidget(speedSlider); // Добавляем ползунок
    sliderLayout->addStretch(); // Добавляем растяжение справа

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(); // Растяжение для размещения ползунка внизу
    mainLayout->addLayout(sliderLayout); // Добавляем ползунок внизу
    setLayout(mainLayout);
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

    // Очистка экрана перед отрисовкой новой модели
    painter.fillRect(rect(), Qt::white);

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
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos();
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // Обновляем целевые углы поворота
        targetRotationX += (event->y() - lastMousePosition.y()) * 0.5;
        targetRotationY += (event->x() - lastMousePosition.x()) * 0.5;
        lastMousePosition = event->pos();
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
    update(); // Обновляем отображение при изменении масштаба
}

void Viewer::updateRotation()
{
    // Плавное изменение углов поворота с учетом скорости вращения
    float deltaX = (targetRotationX - rotationX) * 0.1f;
    float deltaY = (targetRotationY - rotationY) * 0.1f;

    rotationX += deltaX * rotationSpeed / 10.0f; // Регулировка по скорости
    rotationY += deltaY * rotationSpeed / 10.0f; // Регулировка по скорости

    // Обновляем отображение
    update();
}

void Viewer::setRotationSpeed(int speed)
{
    rotationSpeed = speed; // Установка новой скорости вращения
}
