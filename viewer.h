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
    void setScale(float scale); // Новый метод для установки масштаба

    void paintEvent(QPaintEvent *event) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    Model *model;
    QPoint lastMousePosition;
    float rotationX;
    float rotationY;
    float scale; // Переменная для хранения масштаба
};

#endif // VIEWER_H
