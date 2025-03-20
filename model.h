#ifndef MODEL_H
#define MODEL_H

#include <QVector>      // Для использования QVector
#include <QVector3D>    // Для работы с 3D векторами

class Model
{
public:
    Model();  // Конструктор
    bool load(const QString &filePath);  // Загрузка модели из файла
    double calculateVolume() const;      // Расчет объема модели
    double calculateProjectionArea() const;  // Расчет площади проекции модели

    // Публичные методы для доступа к данным
    const QVector<QVector3D>& getVertices() const;
    const QVector<QVector<int>>& getFaces() const;

    // Методы для вращения модели
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);

private:
    QVector<QVector3D> vertices;  // Список вершин модели
    QVector<QVector<int>> faces;  // Список граней модели (индексы вершин)
};

#endif // MODEL_H
