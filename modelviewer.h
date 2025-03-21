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

private:
    Model *model;
    Viewer *viewer;
};

#endif // MODELVIEWER_H
