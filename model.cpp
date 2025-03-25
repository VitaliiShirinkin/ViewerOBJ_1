#include "model.h"
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <QSet>

// Конструктор класса Model
Model::Model() {}

// Метод для загрузки модели из файла
bool Model::load(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    vertices.clear();
    faces.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        if (parts[0] == "v") {
            float x = parts[1].toFloat() / 1000.0f;
            float y = parts[2].toFloat() / 1000.0f;
            float z = parts[3].toFloat() / 1000.0f;
            vertices.append(QVector3D(x, y, z));
        }
        else if (parts[0] == "f") {
            QVector<int> face;
            for (int i = 1; i < parts.size(); ++i) {
                face.append(parts[i].split("/")[0].toInt() - 1);
            }
            faces.append(face);
        }
    }

    file.close();
    return true;
}

// Метод для вычисления объема модели
double Model::calculateVolume() const {
    double volume = 0.0;
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue;
        const QVector3D &v0 = vertices[face[0]];
        const QVector3D &v1 = vertices[face[1]];
        const QVector3D &v2 = vertices[face[2]];
        volume += v0.x() * v1.y() * v2.z() + v1.x() * v2.y() * v0.z() + v2.x() * v0.y() * v1.z()
                - v0.z() * v1.y() * v2.x() - v1.z() * v2.y() * v0.x() - v2.z() * v0.y() * v1.x();
    }
    return std::abs(volume) / 6.0;
}
double Model::calculateProjectionArea() const {
    double area = 0.0;

    // Направление проекции (в данном случае проекция на плоскость XY, поэтому направление - ось Z)
    QVector3D projectionDirection(0, 0, 1);

    // Проходим по всем граням
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue; // Пропускаем неполные грани

        // Разбиваем грань на треугольники и вычисляем площадь для каждого
        const QVector3D &v0 = vertices[face[0]]; // Первая вершина
        for (int i = 1; i < face.size() - 1; ++i) {
            const QVector3D &v1 = vertices[face[i]];
            const QVector3D &v2 = vertices[face[i + 1]];

            // Вычисляем векторы граней
            QVector3D edge1 = v1 - v0;
            QVector3D edge2 = v2 - v0;

            // Находим нормаль к плоскости, образованной треугольником
            QVector3D normal = QVector3D::crossProduct(edge1, edge2);

            // Вычисляем площадь треугольника
            double triangleArea = normal.length() / 2.0;

            // Вычисляем косинус угла между нормалью и направлением проекции
            double cosTheta = QVector3D::dotProduct(normal.normalized(), projectionDirection);

            // Площадь проекции треугольника равна площади треугольника, умноженной на |cos(theta)|
            // Учитываем только те грани, которые видны в проекции (cosTheta > 0)
            if (cosTheta > 0) {
                area += triangleArea * cosTheta;
            }
        }
    }

    return area;
}

        // Метод для получения размеров модели
        QVector3D Model::getModelDimensions() const {
            if (vertices.isEmpty()) {
                return QVector3D(0, 0, 0);
            }

            float minX = vertices[0].x(), maxX = vertices[0].x();
            float minY = vertices[0].y(), maxY = vertices[0].y();
            float minZ = vertices[0].z(), maxZ = vertices[0].z();

            for (const QVector3D &vertex : vertices) {
                minX = qMin(minX, vertex.x());
                maxX = qMax(maxX, vertex.x());
                minY = qMin(minY, vertex.y());
                maxY = qMax(maxY, vertex.y());
                minZ = qMin(minZ, vertex.z());
                maxZ = qMax(maxZ, vertex.z());
            }

            return QVector3D(maxX - minX, maxY - minY, maxZ - minZ);
        }

        // Метод для получения списка вершин
        const QVector<QVector3D>& Model::getVertices() const {
            return vertices;
        }

        // Метод для получения списка граней
        const QVector<QVector<int>>& Model::getFaces() const {
            return faces;
        }

        // Метод для поворота модели по оси X
        void Model::rotateX(float angle) {
            float rad = qDegreesToRadians(angle);
            float cosA = cos(rad);
            float sinA = sin(rad);

            for (QVector3D &vertex : vertices) {
                float y = vertex.y() * cosA - vertex.z() * sinA;
                float z = vertex.y() * sinA + vertex.z() * cosA;
                vertex.setY(y);
                vertex.setZ(z);
            }
        }

        // Метод для поворота модели по оси Y
        void Model::rotateY(float angle) {
            float rad = qDegreesToRadians(angle);
            float cosA = cos(rad);
            float sinA = sin(rad);

            for (QVector3D &vertex : vertices) {
                float x = vertex.x() * cosA + vertex.z() * sinA;
                float z = -vertex.x() * sinA + vertex.z() * cosA;
                vertex.setX(x);
                vertex.setZ(z);
            }
        }

        // Метод для поворота модели по оси Z
        void Model::rotateZ(float angle) {
            float rad = qDegreesToRadians(angle);
            float cosA = cos(rad);
            float sinA = sin(rad);

            for (QVector3D &vertex : vertices) {
                float x = vertex.x() * cosA - vertex.y() * sinA;
                float y = vertex.x() * sinA + vertex.y() * cosA;
                vertex.setX(x);
                vertex.setY(y);
            }
        }

        // Метод для перемещения модели
        void Model::translate(float dx, float dy, float dz) {
            for (QVector3D &vertex : vertices) {
                vertex.setX(vertex.x() + dx);
                vertex.setY(vertex.y() + dy);
                vertex.setZ(vertex.z() + dz);
            }
        }
