// File: main.cpp
// Подключаем основной класс приложения Qt
#include <QApplication>
// Подключаем заголовочный файл главного окна
#include "mainwindow.h"

// Точка входа в программу
int main(int argc, char *argv[])
{
    // Создаем объект приложения
    QApplication app(argc, argv);
    // Создаем объект главного окна
    MainWindow window;
    // Отображаем главное окно
    window.show();
    // Запускаем основной цикл приложения
    return app.exec();
}


// File: mainwindow.cpp
// Подключаем заголовочный файл главного окна
#include "mainwindow.h"
// Подключаем необходимые классы для работы с меню, диалогами и виджетами
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

// Конструктор главного окна
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), modelViewer(new ModelViewer(this))
{
    // Устанавливаем заголовок окна
    setWindowTitle("Viewer Obj");
    // Устанавливаем центральный виджет
    setCentralWidget(modelViewer);

    // Создаем меню "Файл"
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    // Добавляем действие "Открыть"
    QAction *openAction = fileMenu->addAction("Открыть");
    // Добавляем действие "Сохранить текст"
    QAction *saveTextAction = fileMenu->addAction("Сохранить текст");
    // Соединяем действие "Открыть" с методом openModel
    connect(openAction, &QAction::triggered, this, &MainWindow::openModel);
    // Соединяем действие "Сохранить текст" с методом saveText
    connect(saveTextAction, &QAction::triggered, this, &MainWindow::saveText);

    // Создаем меню "Трансформации"
    QMenu *transformMenu = menuBar()->addMenu("Трансформации");
    // Добавляем действие "Повернуть модель"
    QAction *rotateAction = transformMenu->addAction("Повернуть модель");
    // Добавляем действие "Переместить модель"
    QAction *translateAction = transformMenu->addAction("Переместить модель");
    // Соединяем действие "Повернуть модель" с методом rotateModel
    connect(rotateAction, &QAction::triggered, this, &MainWindow::rotateModel);
    // Соединяем действие "Переместить модель" с методом translateModel
    connect(translateAction, &QAction::triggered, this, &MainWindow::translateModel);
}

// Деструктор главного окна
MainWindow::~MainWindow() {}

// Метод для открытия модели
void MainWindow::openModel()
{
    // Открываем диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть модель", "", "OBJ Files (*.obj)");

    if (!filePath.isEmpty()) {
        // Если модель успешно загружена
        if (modelViewer->loadModel(filePath)) {
            // Обновляем заголовок окна
            updateWindowTitle();
        } else {
            // Выводим сообщение об ошибке
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить модель.");
        }
    } else {
        // Выводим предупреждение, если файл не выбран
        QMessageBox::warning(this, "Предупреждение", "Файл не выбран.");
    }
}

// Метод для сохранения текста
void MainWindow::saveText()
{
    // Открываем диалог сохранения файла
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить текст", "", "Text Files (*.txt)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        // Если файл успешно открыт
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            // Записываем заголовок окна в файл
            stream << windowTitle();
            file.close();
        } else {
            // Выводим сообщение об ошибке, если не удалось открыть файл
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
        }
    }
}

// Метод для поворота модели
void MainWindow::rotateModel()
{
    // Создаем диалоговое окно для ввода углов поворота
    QDialog dialog(this);
    dialog.setWindowTitle("Поворот модели");

    // Создаем форму для ввода данных
    QFormLayout form(&dialog);
    QLineEdit angleXInput(&dialog);
    QLineEdit angleYInput(&dialog);
    QLineEdit angleZInput(&dialog);
    QPushButton applyButton("Применить", &dialog);

    // Добавляем поля ввода в форму
    form.addRow("Угол поворота X (градусы):", &angleXInput);
    form.addRow("Угол поворота Y (градусы):", &angleYInput);
    form.addRow("Угол поворота Z (градусы):", &angleZInput);
    form.addRow(&applyButton);

    // Соединяем кнопку "Применить" с обработчиком
    connect(&applyButton, &QPushButton::clicked, [&]() {
        // Получаем введенные значения углов
        float angleX = angleXInput.text().toFloat();
        float angleY = angleYInput.text().toFloat();
        float angleZ = angleZInput.text().toFloat();

        // Выполняем поворот модели
        modelViewer->rotateModel(angleX, angleY, angleZ);
        // Обновляем заголовок окна
        updateWindowTitle();
        // Закрываем диалоговое окно
        dialog.close();
    });

    // Отображаем диалоговое окно
    dialog.exec();
}

// Метод для перемещения модели
void MainWindow::translateModel()
{
    // Создаем диалоговое окно для ввода смещения
    QDialog dialog(this);
    dialog.setWindowTitle("Перемещение модели");

    // Создаем форму для ввода данных
    QFormLayout form(&dialog);
    QLineEdit dxInput(&dialog);
    QLineEdit dyInput(&dialog);
    QLineEdit dzInput(&dialog);
    QPushButton applyButton("Применить", &dialog);

    // Добавляем поля ввода в форму
    form.addRow("Смещение по X (м):", &dxInput);
    form.addRow("Смещение по Y (м):", &dyInput);
    form.addRow("Смещение по Z (м):", &dzInput);
    form.addRow(&applyButton);

    // Соединяем кнопку "Применить" с обработчиком
    connect(&applyButton, &QPushButton::clicked, [&]() {
        // Получаем введенные значения смещения
        float dx = dxInput.text().toFloat();
        float dy = dyInput.text().toFloat();
        float dz = dzInput.text().toFloat();

        // Выполняем перемещение модели
        modelViewer->translateModel(dx, dy, dz);
        // Обновляем заголовок окна
        updateWindowTitle();
        // Закрываем диалоговое окно
        dialog.close();
    });

    // Отображаем диалоговое окно
    dialog.exec();
}
// Метод для обновления заголовка окна
void MainWindow::updateWindowTitle()
{
    // Получаем размеры модели
    QVector3D dimensions = modelViewer->getModelDimensions();
    // Вычисляем объем модели
    double volume = modelViewer->calculateVolume();
    // Вычисляем площадь проекции модели
    double area = modelViewer->calculateProjectionArea();

    // Формируем новый заголовок окна
    QString title = QString("Viewer Obj | Модель: %1 | Размеры: %2x%3x%4 м | Объем: %5 м³ | Площадь проекции: %6 м²")
                    .arg("Модель") // Здесь можно добавить имя модели, если нужно
                    .arg(dimensions.x(), 0, 'f', 2)
                    .arg(dimensions.y(), 0, 'f', 2)
                    .arg(dimensions.z(), 0, 'f', 2)
                    .arg(volume, 0, 'f', 2)
                    .arg(area, 0, 'f', 2);
    // Устанавливаем новый заголовок окна
    setWindowTitle(title);
}




// File: mainwindow.h
// Защита от повторного включения заголовочного файла
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Подключаем базовый класс для главного окна
#include <QMainWindow>
// Подключаем заголовочный файл для работы с моделью и отображением
#include "modelviewer.h"

// Класс главного окна
class MainWindow : public QMainWindow
{
    Q_OBJECT  // Макрос для поддержки сигналов и слотов

public:
    // Конструктор с параметром родительского виджета
    MainWindow(QWidget *parent = nullptr);
    // Деструктор
    ~MainWindow();

private slots:
    // Слот для открытия модели
    void openModel();
    // Слот для сохранения текста
    void saveText();
    // Слот для поворота модели
    void rotateModel();
    // Слот для перемещения модели
    void translateModel();
    // Слот для обновления заголовка окна
    void updateWindowTitle();

private:
    // Указатель на объект для работы с моделью и отображением
    ModelViewer *modelViewer;
};

// Завершение защиты от повторного включения
#endif // MAINWINDOW_H


// File: model.cpp
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
            area += triangleArea * std::abs(cosTheta);
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


// File: model.h
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


// File: modelviewer.cpp
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


// File: modelviewer.h
#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include <QWidget>
#include "model.h"
#include "viewer.h"

class ModelViewer : public QWidget
{
    Q_OBJECT

public:
    ModelViewer(QWidget *parent = nullptr);
    bool loadModel(const QString &filePath);

    QVector3D getModelDimensions() const;
    double calculateVolume() const;
    double calculateProjectionArea() const;

    void rotateModel(float angleX, float angleY, float angleZ);
    void translateModel(float dx, float dy, float dz);

private:
    Model *model;
    Viewer *viewer;
};

#endif // MODELVIEWER_H


// File: viewer.cpp
#include "viewer.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QWheelEvent>

// Конструктор класса Viewer
Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0), selectedVertexIndex(-1)
{
    setMinimumSize(400, 400); // Устанавливаем минимальный размер окна
    setMouseTracking(true); // Включаем отслеживание движения мыши
}

// Метод для установки модели
void Viewer::setModel(Model *model) {
    this->model = model; // Устанавливаем модель
    update(); // Обновляем отображение
}

// Метод для установки масштаба
void Viewer::setScale(float scale) {
    this->scale = scale; // Устанавливаем масштаб
    update(); // Обновляем отображение
}

// Метод для обработки событий рисования
void Viewer::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // Включаем сглаживание

    if (!model) {
        // Если модель не загружена, выводим сообщение
        painter.drawText(rect(), Qt::AlignCenter, "Модель не загружена");
        return;
    }

    // Очищаем экран белым цветом
    painter.fillRect(rect(), Qt::white);

    // Отрисовка осей координат
    int margin = 20; // Отступ от краев экрана
    int axisLength = qMax(width(), height()); // Длина осей равна максимальному размеру окна

    // Ось X (красная)
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(margin, height() - margin, width() - margin, height() - margin); // Ось X
    painter.drawText(width() - margin - 10, height() - margin + 15, "X");

    // Ось Y (зеленая)
    painter.setPen(QPen(Qt::green, 2));
    painter.drawLine(margin, height() - margin, margin, margin); // Ось Y
    painter.drawText(margin - 10, margin + 15, "Y");
    // Ось Z (синяя)
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(margin, height() - margin, margin + axisLength / 2, height() - margin - axisLength / 2); // Ось Z
    painter.drawText(margin + axisLength / 2 + 10, height() - margin - axisLength / 2 - 10, "Z");

    // Получаем вершины и грани модели
    const QVector<QVector3D> &vertices = model->getVertices();
    const QVector<QVector<int>> &faces = model->getFaces();

    // Отрисовываем вершины
    painter.setPen(QPen(Qt::black, 2));
    for (int i = 0; i < vertices.size(); ++i) {
        const QVector3D &vertex = vertices[i];

        // Применяем поворот и масштабирование
        float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
        float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
        float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
        z = vertex.y() * sin(rotationX) + z * cos(rotationX);

        // Преобразуем координаты в экранные
        QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));

        // Отрисовываем вершину
        if (i == selectedVertexIndex) {
            // Выделяем выбранную вершину красным цветом
            painter.setPen(QPen(Qt::red, 4));
            painter.drawEllipse(point, 5, 5);
            painter.setPen(QPen(Qt::black, 2));
        } else {
            painter.drawEllipse(point, 3, 3);
        }
    }

    // Отрисовываем грани
    painter.setPen(QPen(Qt::blue, 2));
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue; // Пропускаем неполные грани
        QPolygonF polygon;
        for (int index : face) {
            const QVector3D &vertex = vertices[index];

            // Применяем поворот и масштабирование
            float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
            float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
            float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
            z = vertex.y() * sin(rotationX) + z * cos(rotationX);

            // Преобразуем координаты в экранные
            QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
            polygon << point; // Добавляем точку в полигон
        }
        painter.drawPolygon(polygon); // Рисуем грань
    }

    // Отображаем координаты и индекс выбранной вершины
    if (selectedVertexIndex != -1) {
        const QVector3D &vertex = vertices[selectedVertexIndex];
        QString info = QString("Вершина [%1]: (%2, %3, %4)")
                          .arg(selectedVertexIndex) // Индекс вершины
                          .arg(vertex.x(), 0, 'f', 2) // Координата X
                          .arg(vertex.y(), 0, 'f', 2) // Координата Y
                          .arg(vertex.z(), 0, 'f', 2); // Координата Z
        painter.setPen(QPen(Qt::black, 2));
        painter.drawText(10, 20, info); // Отображаем информацию в верхнем левом углу
    }
}

// Метод для обработки нажатий кнопок мыши
void Viewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos(); // Запоминаем позицию мыши

        if (model) {
            const QVector<QVector3D> &vertices = model->getVertices();
            int closestVertexIndex = -1;
            float minDistance = 10.0f; // Максима
            // Ищем ближайшую вершину к месту клика
            for (int i = 0; i < vertices.size(); ++i) {
                const QVector3D &vertex = vertices[i];

                // Применяем поворот и масштабирование
                float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
                float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
                float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
                z = vertex.y() * sin(rotationX) + z * cos(rotationX);

                // Преобразуем координаты в экранные
                QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));

                // Вычисляем расстояние от курсора мыши до вершины
                float distance = QLineF(event->pos(), point).length();
                if (distance < minDistance) {
                    minDistance = distance;
                    closestVertexIndex = i;
                }
            }

            // Если найдена ближайшая вершина, выбираем её
            if (closestVertexIndex != -1) {
                selectedVertexIndex = closestVertexIndex;
                update(); // Обновляем отображение
            }
        }
    }
}

// Метод для обработки перемещения мыши
void Viewer::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // Вычисляем изменение углов поворота
        rotationX += (event->y() - lastMousePosition.y()) * 0.01;
        rotationY += (event->x() - lastMousePosition.x()) * 0.01;
        lastMousePosition = event->pos(); // Обновляем позицию мыши
        update(); // Обновляем отображение
    }
}

// Метод для обработки событий прокрутки колесика мыши
void Viewer::wheelEvent(QWheelEvent *event) {
    // Масштабирование модели
    if (event->angleDelta().y() > 0) {
        scale *= 1.1; // Увеличение масштаба
    } else {
        scale /= 1.1; // Уменьшение масштаба
    }
    update(); // Обновляем отображение
}


// File: viewer.h
#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget> // Подключаем основной класс для создания виджетов
#include "model.h" // Подключаем файл модели, который будет использоваться в Viewer

// Объявляем класс Viewer, который наследует от QWidget
class Viewer : public QWidget
{
    Q_OBJECT // Макрос, необходимый для использования сигналов и слотов в Qt

public:
    // Конструктор класса Viewer, принимает родительский виджет
    explicit Viewer(QWidget *parent = nullptr);

    // Метод для установки модели, с которой будет работать Viewer
    void setModel(Model *model);

    // Метод для установки масштаба отображаемой модели
    void setScale(float scale);

protected:
    // Переопределяем метод для обработки событий рисования
    void paintEvent(QPaintEvent *event) override;

    // Переопределяем метод для обработки нажатий кнопок мыши
    void mousePressEvent(QMouseEvent *event) override;

    // Переопределяем метод для обработки перемещения мыши
    void mouseMoveEvent(QMouseEvent *event) override;

    // Переопределяем метод для обработки событий прокрутки колесика мыши
    void wheelEvent(QWheelEvent *event) override;

private:
    Model *model; // Указатель на объект модели, который будет отображаться
    QPoint lastMousePosition; // Хранит последнюю позицию курсора мыши
    float rotationX; // Угол поворота модели по оси X
    float rotationY; // Угол поворота модели по оси Y
    float scale; // Масштаб модели для отображения
    int selectedVertexIndex; // Индекс выбранной вершины модели (если есть)
};

#endif // VIEWER_H // Завершаем условную компиляцию