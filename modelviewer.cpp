#include "modelviewer.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

ModelViewer::ModelViewer(QWidget *parent)
    : QWidget(parent), model(new Model()), viewer(new Viewer(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(viewer);
    setLayout(layout);
}

bool ModelViewer::loadModel(const QString &filePath)
{
    if (model->load(filePath)) {
        viewer->setModel(model);
        // Автоматическое масштабирование при загрузке модели
        QVector3D dimensions = model->getModelDimensions();
        float maxDimension = qMax(dimensions.x(), qMax(dimensions.y(), dimensions.z()));
        if (maxDimension > 0) {
            viewer->setScale(1.0f / maxDimension * 200); // Масштабирование для полного отображения
        }
        return true;
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить модель.");
        return false;
    }
}

QVector3D ModelViewer::getModelDimensions() const {
    return model->getModelDimensions();
}

double ModelViewer::calculateVolume() const {
    return model->calculateVolume();
}

double ModelViewer::calculateProjectionArea() const {
    return model->calculateProjectionArea();
}
