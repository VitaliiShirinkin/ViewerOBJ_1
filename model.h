#ifndef MODEL_H
#define MODEL_H

#include <QVector>
#include <QVector3D>
#include <QSet>
#include <QString>
#include <cmath>

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
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    void translate(float dx, float dy, float dz);

private:
    QVector<QVector3D> vertices; // Список вершин модели
    QVector<QVector<int>> faces; // Список граней модели
};

#endif // MODEL_H
