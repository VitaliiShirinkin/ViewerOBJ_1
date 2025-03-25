#include "modelviewer.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

// Конструктор класса ModelViewer
ModelViewer::ModelViewer(QWidget *parent)
    : QWidget(parent), model(new Model()), viewer(new Viewer(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(viewer);
    setLayout(layout);
}

// Метод для загрузки модели из файла
bool ModelViewer::loadModel(const QString &filePath)
{
    if (model->load(filePath)) {
        viewer->setModel(model);

        QVector3D dimensions = model->getModelDimensions();
        float maxDimension = qMax(dimensions.x(), qMax(dimensions.y(), dimensions.z()));

        if (maxDimension > 0) {
            viewer->setScale(1.0f / maxDimension * 200);
        }

        return true;
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить модель.");
        return false;
    }
}

// Метод для получения размеров модели
QVector3D ModelViewer::getModelDimensions() const {
    return model->getModelDimensions();
}

// Метод для вычисления объема модели
double ModelViewer::calculateVolume() const {
    return model->calculateVolume();
}

// Метод для вычисления площади проекции модели
double ModelViewer::calculateProjectionArea() const {
    return model->calculateProjectionArea();
}

// Метод для вращения модели на заданные углы по осям X, Y и Z
void ModelViewer::rotateModel(float angleX, float angleY, float angleZ) {
    model->rotateX(angleX);
    model->rotateY(angleY);
    model->rotateZ(angleZ);
    viewer->update();
}

// Метод для перемещения модели на заданные расстояния по осям X, Y и Z
void ModelViewer::translateModel(float dx, float dy, float dz) {
    model->translate(dx, dy, dz);
    viewer->update();
}
