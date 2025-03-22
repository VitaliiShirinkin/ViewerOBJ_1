#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QVector3D>

class Model
{
public:
    Model();
    bool load(const QString &filePath);
    double calculateVolume() const;
    double calculateProjectionArea() const;
    QVector3D getModelDimensions() const;
    const QVector<QVector3D>& getVertices() const;
    const QVector<QVector<int>>& getFaces() const;

    // Поворот модели
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);

    // Перемещение модели
    void translate(float dx, float dy, float dz);

private:
    QVector<QVector3D> vertices; // Объявление переменной vertices
    QVector<QVector<int>> faces;
};

#endif // MODEL_H
