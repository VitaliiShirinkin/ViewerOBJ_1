#include "model.h"          // Подключение заголовочного файла модели
#include <QFile>            // Для работы с файлами
#include <QTextStream>      // Для чтения текстовых данных из файлов
#include <cmath>            // Для математических функций, таких как abs, sin и cos

Model::Model() {}  // Конструктор (пока пустой)

// Загрузка модели из файла формата OBJ
bool Model::load(const QString &filePath)
{
    QFile file(filePath);  // Открытие файла по указанному пути
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  // Проверка на успешное открытие
        return false;  // Если файл не удалось открыть, возвращаем false

    QTextStream in(&file);  // Создание текстового потока для чтения данных из файла
    while (!in.atEnd()) {  // Пока не достигнут конец файла
        QString line = in.readLine();  // Чтение строки из файла
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);  // Разделение строки на части
        if (parts.isEmpty()) continue;  // Пропуск пустых строк

        if (parts[0] == "v") {  // Если строка начинается с "v", это вершина
            float x = parts[1].toFloat();  // Чтение координаты x
            float y = parts[2].toFloat();  // Чтение координаты y
            float z = parts[3].toFloat();  // Чтение координаты z
            vertices.append(QVector3D(x, y, z));  // Добавление вершины в список
        } else if (parts[0] == "f") {  // Если строка начинается с "f", это грань
            QVector<int> face;  // Создание вектора для хранения индексов вершин грани
            for (int i = 1; i < parts.size(); ++i) {  // Проход по всем элементам грани
                face.append(parts[i].split("/")[0].toInt() - 1);  // Добавление индекса вершины (уменьшаем на 1 для нулевой индексации)
            }
            faces.append(face);  // Добавление грани в список
        }
    }

    file.close();  // Закрытие файла после завершения чтения
    return true;   // Возвращаем true, если загрузка прошла успешно
}
// Расчет объема модели
double Model::calculateVolume() const
{
    double volume = 0.0;  // Переменная для хранения объема
    for (const QVector<int> &face : faces) {  // Проход по всем граням
        if (face.size() < 3) continue;  // Пропуск, если грань состоит менее чем из 3 вершин
        const QVector3D &v0 = vertices[face[0]];  // Первая вершина грани
        const QVector3D &v1 = vertices[face[1]];  // Вторая вершина грани
        const QVector3D &v2 = vertices[face[2]];  // Третья вершина грани
        // Вычисление объема по формуле
        volume += v0.x() * v1.y() * v2.z() + v1.x() * v2.y() * v0.z() + v2.x() * v0.y() * v1.z()
                - v0.z() * v1.y() * v2.x() - v1.z() * v2.y() * v0.x() - v2.z() * v0.y() * v1.x();
    }
    return std::abs(volume) / 6.0;  // Возвращаем абсолютное значение объема, деленное на 6
}

// Расчет площади проекции модели
double Model::calculateProjectionArea() const
{
    double area = 0.0;  // Переменная для хранения площади
    for (const QVector<int> &face : faces) {  // Проход по всем граням
        if (face.size() < 3) continue;  // Пропуск, если грань состоит менее чем из 3 вершин
        const QVector3D &v0 = vertices[face[0]];  // Первая вершина грани
        const QVector3D &v1 = vertices[face[1]];  // Вторая вершина грани
        const QVector3D &v2 = vertices[face[2]];  // Третья вершина грани
        // Вычисление площади треугольника по формуле
        area += std::abs((v1.x() - v0.x()) * (v2.y() - v0.y()) - (v2.x() - v0.x()) * (v1.y() - v0.y())) / 2.0;
    }
    return area;  // Возвращаем общую площадь проекции
}

// Реализация публичных методов для доступа к данным
const QVector<QVector3D>& Model::getVertices() const
{
    return vertices;  // Возвращаем список вершин
}
const QVector<QVector<int>>& Model::getFaces() const
{
    return faces;  // Возвращаем список граней
}

// Методы для вращения модели
void Model::rotateX(float angle)
{
    float rad = qDegreesToRadians(angle);  // Преобразование градусов в радианы
    for (QVector3D &vertex : vertices) {  // Проход по всем вершинам
        float y = vertex.y() * cos(rad) - vertex.z() * sin(rad);  // Новая координата y
        float z = vertex.y() * sin(rad) + vertex.z() * cos(rad);  // Новая координата z
        vertex.setY(y);  // Установка новой координаты y
        vertex.setZ(z);  // Установка новой координаты z
    }
}

void Model::rotateY(float angle)
{
    float rad = qDegreesToRadians(angle);  // Преобразование градусов в радианы
    for (QVector3D &vertex : vertices) {  // Проход по всем вершинам
        float x = vertex.x() * cos(rad) + vertex.z() * sin(rad);  // Новая координата x
        float z = -vertex.x() * sin(rad) + vertex.z() * cos(rad);  // Новая координата z
        vertex.setX(x);  // Установка новой координаты x
        vertex.setZ(z);  // Установка новой координаты z
    }
}

void Model::rotateZ(float angle)
{
    float rad = qDegreesToRadians(angle);  // Преобразование градусов в радианы
    for (QVector3D &vertex : vertices) {  // Проход по всем вершинам
        float x = vertex.x() * cos(rad) - vertex.y() * sin(rad);  // Новая координата x
        float y = vertex.x() * sin(rad) + vertex.y() * cos(rad);  // Новая координата y
        vertex.setX(x);  // Установка новой координаты x
        vertex.setY(y);  // Установка новой координаты y
    }
}
