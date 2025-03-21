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
    QVector3D getModelDimensions() const; // Новый метод для получения размеров модели

    const QVector<QVector3D>& getVertices() const;
    const QVector<QVector<int>>& getFaces() const;

    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);

private:
    QVector<QVector3D> vertices;
    QVector<QVector<int>> faces;
};

#endif // MODEL_H
