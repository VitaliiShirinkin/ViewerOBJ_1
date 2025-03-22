#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include <QWidget>
#include "model.h"
#include "viewer.h"

class ModelViewer : public QWidget
{
    Q_OBJECT

public:
    ModelViewer(QWidget *parent = nullptr);
    bool loadModel(const QString &filePath);

    QVector3D getModelDimensions() const;
    double calculateVolume() const;
    double calculateProjectionArea() const;

    // Методы для поворота и перемещения модели
    void rotateModel(float angleX, float angleY, float angleZ);
    void translateModel(float dx, float dy, float dz);

private:
    Model *model;
    Viewer *viewer;
};

#endif // MODELVIEWER_H
