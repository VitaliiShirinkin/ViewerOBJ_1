#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include "model.h"

class Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer(QWidget *parent = nullptr);
    void setModel(Model *model);
    void setScale(float scale);

signals:
    void vertexSelected(int index, const QVector3D &vertex); // Сигнал для передачи информации о выделенной вершине

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    Model *model;
    QPointF lastMousePosition; // Изменяем тип на QPointF
    float rotationX;
    float rotationY;
    float scale;
    int selectedVertexIndex;

};

#endif // VIEWER_H
