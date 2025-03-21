#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QTimer>
#include <QSlider> // Добавлен для регулировки скорости вращения
#include "model.h"

class Viewer : public QWidget
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    void setModel(Model *model);
    void setScale(float scale);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void updateRotation(); // Слот для обновления углов поворота
    void setRotationSpeed(int speed); // Слот для установки скорости вращения

private:
    Model *model;
    QPoint lastMousePosition;
    float rotationX;
    float rotationY;
    float scale;
    QTimer *rotationTimer; // Таймер для плавного вращения
    float targetRotationX; // Целевой угол поворота по X
    float targetRotationY; // Целевой угол поворота по Y
    int rotationSpeed; // Скорость вращения
    QSlider *speedSlider; // Ползунок для регулировки скорости
};

#endif // VIEWER_H
