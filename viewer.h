#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
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

private:
    Model *model; // Указатель на модель
    QPoint lastMousePosition; // Последняя позиция мыши
    float rotationX; // Угол поворота по оси X
    float rotationY; // Угол поворота по оси Y
    float scale; // Масштаб модели
};

#endif // VIEWER_H
