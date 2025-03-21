#ifndef MODEL_H
#define MODEL_H

#include <QVector> // Для использования QVector
#include <QVector3D> // Для работы с 3D-векторами

class Model
{
public:
    // Конструктор
    Model();

    // Загрузка модели из файла
    bool load(const QString &filePath);

    // Вычисление объема модели
    double calculateVolume() const;

    // Вычисление площади проекции модели
    double calculateProjectionArea() const;

    // Получение размеров модели (ширина, высота, глубина)
    QVector3D getModelDimensions() const;

    // Получение списка вершин модели
    const QVector<QVector3D>& getVertices() const;

    // Получение списка граней модели
    const QVector<QVector<int>>& getFaces() const;

    // Поворот модели вокруг оси X
    void rotateX(float angle);

    // Поворот модели вокруг оси Y
    void rotateY(float angle);

    // Поворот модели вокруг оси Z
    void rotateZ(float angle);

private:
    QVector<QVector3D> vertices; // Список вершин модели
    QVector<QVector<int>> faces; // Список граней модели
};

#endif // MODEL_H
