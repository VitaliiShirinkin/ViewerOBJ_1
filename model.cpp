#include "model.h"
#include <QFile>
#include <QTextStream>
#include <cmath>

Model::Model() {}

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
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue;
        const QVector3D &v0 = vertices[face[0]];
        const QVector3D &v1 = vertices[face[1]];
        const QVector3D &v2 = vertices[face[2]];
        area += std::abs((v1.x() - v0.x()) * (v2.y() - v0.y()) - (v2.x() - v0.x()) * (v1.y() - v0.y())) / 2.0;
    }
    return area;
}

QVector3D Model::getModelDimensions() const {
    if (vertices.isEmpty()) return QVector3D(0, 0, 0);

    float minX = vertices[0].x(), maxX = vertices[0].x();
    float minY = vertices[0].y(), maxY = vertices[0].y();
    float minZ = vertices[0].z(), maxZ = vertices[0].z();

    for (const QVector3D &vertex : vertices) {
        if (vertex.x() < minX) minX = vertex.x();
        if (vertex.x() > maxX) maxX = vertex.x();
        if (vertex.y() < minY) minY = vertex.y();
        if (vertex.y() > maxY) maxY = vertex.y();
        if (vertex.z() < minZ) minZ = vertex.z();
        if (vertex.z() > maxZ) maxZ = vertex.z();
    }

    return QVector3D(maxX - minX, maxY - minY, maxZ - minZ);
}

const QVector<QVector3D>& Model::getVertices() const {
    return vertices;
}

const QVector<QVector<int>>& Model::getFaces() const {
    return faces;
}

void Model::rotateX(float angle) {
    float rad = qDegreesToRadians(angle);
    for (QVector3D &vertex : vertices) {
        float y = vertex.y() * cos(rad) - vertex.z() * sin(rad);
        float z = vertex.y() * sin(rad) + vertex.z() * cos(rad);
        vertex.setY(y);
        vertex.setZ(z);
    }
}

void Model::rotateY(float angle) {
    float rad = qDegreesToRadians(angle);
    for (QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rad) + vertex.z() * sin(rad);
        float z = -vertex.x() * sin(rad) + vertex.z() * cos(rad);
        vertex.setX(x);
        vertex.setZ(z);
    }
}

void Model::rotateZ(float angle) {
    float rad = qDegreesToRadians(angle);
    for (QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rad) - vertex.y() * sin(rad);
        float y = vertex.x() * sin(rad) + vertex.y() * cos(rad);
        vertex.setX(x);
        vertex.setY(y);
    }
}

void Model::translate(float dx, float dy, float dz) {
    for (QVector3D &vertex : vertices) {
        vertex.setX(vertex.x() + dx);
        vertex.setY(vertex.y() + dy);
        vertex.setZ(vertex.z() + dz);
    }
}

