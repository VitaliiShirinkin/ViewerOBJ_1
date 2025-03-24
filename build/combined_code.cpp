// File: main.cpp
#include <QApplication>  // Основной класс приложения Qt
#include "mainwindow.h"  // Заголовочный файл главного окна

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // Создание объекта приложения
    MainWindow window;             // Создание объекта главного окна
    window.show();                 // Отображение главного окна
    return app.exec();             // Запуск основного цикла приложения
}


// File: mainwindow.cpp
#include "mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), modelViewer(new ModelViewer(this))
{
    setWindowTitle("3D Model Viewer");
    setCentralWidget(modelViewer);

    QMenu *fileMenu = menuBar()->addMenu("Файл");
    QAction *openAction = fileMenu->addAction("Открыть");
    QAction *saveTextAction = fileMenu->addAction("Сохранить текст");
    connect(openAction, &QAction::triggered, this, &MainWindow::openModel);
    connect(saveTextAction, &QAction::triggered, this, &MainWindow::saveText);

    QMenu *transformMenu = menuBar()->addMenu("Трансформации");
    QAction *rotateAction = transformMenu->addAction("Повернуть модель");
    QAction *translateAction = transformMenu->addAction("Переместить модель");
    connect(rotateAction, &QAction::triggered, this, &MainWindow::rotateModel);
    connect(translateAction, &QAction::triggered, this, &MainWindow::translateModel);
}

MainWindow::~MainWindow() {}

void MainWindow::openModel()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть модель", "", "OBJ Files (*.obj)");

    if (!filePath.isEmpty()) {
        if (modelViewer->loadModel(filePath)) {
            updateWindowTitle();  // Обновляем заголовок окна после загрузки модели
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить модель.");
        }
    } else {
        QMessageBox::warning(this, "Предупреждение", "Файл не выбран.");
    }
}
void MainWindow::saveText()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить текст", "", "Text Files (*.txt)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << windowTitle();
            file.close();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
        }
    }
}

void MainWindow::rotateModel()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Поворот модели");

    QFormLayout form(&dialog);
    QLineEdit angleXInput(&dialog);
    QLineEdit angleYInput(&dialog);
    QLineEdit angleZInput(&dialog);
    QPushButton applyButton("Применить", &dialog);

    form.addRow("Угол поворота X (градусы):", &angleXInput);
    form.addRow("Угол поворота Y (градусы):", &angleYInput);
    form.addRow("Угол поворота Z (градусы):", &angleZInput);
    form.addRow(&applyButton);

    connect(&applyButton, &QPushButton::clicked, [&]() {
        float angleX = angleXInput.text().toFloat();
        float angleY = angleYInput.text().toFloat();
        float angleZ = angleZInput.text().toFloat();

        modelViewer->rotateModel(angleX, angleY, angleZ);
        updateWindowTitle();  // Обновляем заголовок окна после поворота
        dialog.close();
    });

    dialog.exec();
}

void MainWindow::translateModel()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Перемещение модели");

    QFormLayout form(&dialog);
    QLineEdit dxInput(&dialog);
    QLineEdit dyInput(&dialog);
    QLineEdit dzInput(&dialog);
    QPushButton applyButton("Применить", &dialog);

    form.addRow("Смещение по X (м):", &dxInput);
    form.addRow("Смещение по Y (м):", &dyInput);
    form.addRow("Смещение по Z (м):", &dzInput);
    form.addRow(&applyButton);

    connect(&applyButton, &QPushButton::clicked, [&]() {
        float dx = dxInput.text().toFloat();
        float dy = dyInput.text().toFloat();
        float dz = dzInput.text().toFloat();

        modelViewer->translateModel(dx, dy, dz);
        updateWindowTitle();  // Обновляем заголовок окна после перемещения
        dialog.close();
    });

    dialog.exec();
}
void MainWindow::updateWindowTitle()
{
    QVector3D dimensions = modelViewer->getModelDimensions();
    double volume = modelViewer->calculateVolume();
    double area = modelViewer->calculateProjectionArea();

    QString title = QString("3D Model Viewer | Модель: %1 | Размеры: %2x%3x%4 м | Объем: %5 м³ | Площадь проекции: %6 м²")
                    .arg("Модель") // Здесь можно добавить имя модели, если нужно
                    .arg(dimensions.x(), 0, 'f', 2)
                    .arg(dimensions.y(), 0, 'f', 2)
                    .arg(dimensions.z(), 0, 'f', 2)
                    .arg(volume, 0, 'f', 2)
                    .arg(area, 0, 'f', 2);
    setWindowTitle(title);
}



// File: mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modelviewer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openModel();
    void saveText();
    void rotateModel(); // Слот для поворота модели
    void translateModel(); // Слот для перемещения модели
    void updateWindowTitle(); // Новый слот для обновления заголовка окна

private:
    ModelViewer *modelViewer;
};

#endif // MAINWINDOW_H


// File: model.cpp
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


// File: model.h
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


// File: modelviewer.cpp
#include "modelviewer.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

ModelViewer::ModelViewer(QWidget *parent)
    : QWidget(parent), model(new Model()), viewer(new Viewer(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(viewer);
    setLayout(layout);
}

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

QVector3D ModelViewer::getModelDimensions() const {
    return model->getModelDimensions();
}

double ModelViewer::calculateVolume() const {
    return model->calculateVolume();
}

double ModelViewer::calculateProjectionArea() const {
    return model->calculateProjectionArea();
}

void ModelViewer::rotateModel(float angleX, float angleY, float angleZ) {
    model->rotateX(angleX);
    model->rotateY(angleY);
    model->rotateZ(angleZ);
    viewer->update(); // Обновляем отображение
}

void ModelViewer::translateModel(float dx, float dy, float dz) {
    model->translate(dx, dy, dz);
    viewer->update(); // Обновляем отображение
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

    // Методы для поворота и перемещения модели
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

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0)
{
    setMinimumSize(400, 400); // Устанавливаем минимальный размер окна
    setMouseTracking(true); // Включаем отслеживание движения мыши
}

void Viewer::setModel(Model *model) {
    this->model = model; // Устанавливаем модель
    update(); // Обновляем отображение
}

void Viewer::setScale(float scale) {
    this->scale = scale; // Устанавливаем масштаб
    update(); // Обновляем отображение
}

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
        if (i == selectedVertexIndex)
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
    }void Viewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos(); // Запоминаем позицию мыши

        if (model) {
            const QVector<QVector3D> &vertices = model->getVertices();
            int closestVertexIndex = -1;
            float minDistance = 10.0f; // Максимальное расстояние для выбора вершины

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

void Viewer::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // Вычисляем изменение углов поворота
        rotationX += (event->y() - lastMousePosition.y()) * 0.01;
        rotationY += (event->x() - lastMousePosition.x()) * 0.01;
        lastMousePosition = event->pos(); // Обновляем позицию мыши
        update(); // Обновляем отображение
    }
}

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

#include <QWidget>
#include "model.h"

class Viewer : public QWidget
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    void setModel(Model *model);
    void setScale(float scale);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    Model *model; // Указатель на модель
    QPoint lastMousePosition; // Последняя позиция мыши
    float rotationX; // Угол поворота по оси X
    float rotationY; // Угол поворота по оси Y
    float scale; // Масштаб модели
    int selectedVertexIndex; // Индекс выбранной вершины
};

#endif // VIEWER_H


// File: combined_code.cpp
// File: main.cpp
#include <QApplication>  // Основной класс приложения Qt
#include "mainwindow.h"  // Заголовочный файл главного окна

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);  // Создание объекта приложения
    MainWindow window;             // Создание объекта главного окна
    window.show();                 // Отображение главного окна
    return app.exec();             // Запуск основного цикла приложения
}


// File: mainwindow.cpp
#include "mainwindow.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), modelViewer(new ModelViewer(this))
{
    setWindowTitle("3D Model Viewer");
    setCentralWidget(modelViewer);

    QMenu *fileMenu = menuBar()->addMenu("Файл");
    QAction *openAction = fileMenu->addAction("Открыть");
    QAction *saveTextAction = fileMenu->addAction("Сохранить текст");
    connect(openAction, &QAction::triggered, this, &MainWindow::openModel);
    connect(saveTextAction, &QAction::triggered, this, &MainWindow::saveText);

    QMenu *transformMenu = menuBar()->addMenu("Трансформации");
    QAction *rotateAction = transformMenu->addAction("Повернуть модель");
    QAction *translateAction = transformMenu->addAction("Переместить модель");
    connect(rotateAction, &QAction::triggered, this, &MainWindow::rotateModel);
    connect(translateAction, &QAction::triggered, this, &MainWindow::translateModel);
}

MainWindow::~MainWindow() {}

void MainWindow::openModel()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть модель", "", "OBJ Files (*.obj)");

    if (!filePath.isEmpty()) {
        if (modelViewer->loadModel(filePath)) {
            updateWindowTitle();  // Обновляем заголовок окна после загрузки модели
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить модель.");
        }
    } else {
        QMessageBox::warning(this, "Предупреждение", "Файл не выбран.");
    }
}
void MainWindow::saveText()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить текст", "", "Text Files (*.txt)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << windowTitle();
            file.close();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
        }
    }
}

void MainWindow::rotateModel()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Поворот модели");

    QFormLayout form(&dialog);
    QLineEdit angleXInput(&dialog);
    QLineEdit angleYInput(&dialog);
    QLineEdit angleZInput(&dialog);
    QPushButton applyButton("Применить", &dialog);

    form.addRow("Угол поворота X (градусы):", &angleXInput);
    form.addRow("Угол поворота Y (градусы):", &angleYInput);
    form.addRow("Угол поворота Z (градусы):", &angleZInput);
    form.addRow(&applyButton);

    connect(&applyButton, &QPushButton::clicked, [&]() {
        float angleX = angleXInput.text().toFloat();
        float angleY = angleYInput.text().toFloat();
        float angleZ = angleZInput.text().toFloat();

        modelViewer->rotateModel(angleX, angleY, angleZ);
        updateWindowTitle();  // Обновляем заголовок окна после поворота
        dialog.close();
    });

    dialog.exec();
}

void MainWindow::translateModel()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Перемещение модели");

    QFormLayout form(&dialog);
    QLineEdit dxInput(&dialog);
    QLineEdit dyInput(&dialog);
    QLineEdit dzInput(&dialog);
    QPushButton applyButton("Применить", &dialog);

    form.addRow("Смещение по X (м):", &dxInput);
    form.addRow("Смещение по Y (м):", &dyInput);
    form.addRow("Смещение по Z (м):", &dzInput);
    form.addRow(&applyButton);

    connect(&applyButton, &QPushButton::clicked, [&]() {
        float dx = dxInput.text().toFloat();
        float dy = dyInput.text().toFloat();
        float dz = dzInput.text().toFloat();

        modelViewer->translateModel(dx, dy, dz);
        updateWindowTitle();  // Обновляем заголовок окна после перемещения
        dialog.close();
    });

    dialog.exec();
}
void MainWindow::updateWindowTitle()
{
    QVector3D dimensions = modelViewer->getModelDimensions();
    double volume = modelViewer->calculateVolume();
    double area = modelViewer->calculateProjectionArea();

    QString title = QString("3D Model Viewer | Модель: %1 | Размеры: %2x%3x%4 м | Объем: %5 м³ | Площадь проекции: %6 м²")
                    .arg("Модель") // Здесь можно добавить имя модели, если нужно
                    .arg(dimensions.x(), 0, 'f', 2)
                    .arg(dimensions.y(), 0, 'f', 2)
                    .arg(dimensions.z(), 0, 'f', 2)
                    .arg(volume, 0, 'f', 2)
                    .arg(area, 0, 'f', 2);
    setWindowTitle(title);
}



// File: mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modelviewer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openModel();
    void saveText();
    void rotateModel(); // Слот для поворота модели
    void translateModel(); // Слот для перемещения модели
    void updateWindowTitle(); // Новый слот для обновления заголовка окна

private:
    ModelViewer *modelViewer;
};

#endif // MAINWINDOW_H


// File: model.cpp
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


// File: model.h
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


// File: modelviewer.cpp
#include "modelviewer.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

ModelViewer::ModelViewer(QWidget *parent)
    : QWidget(parent), model(new Model()), viewer(new Viewer(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(viewer);
    setLayout(layout);
}

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

QVector3D ModelViewer::getModelDimensions() const {
    return model->getModelDimensions();
}

double ModelViewer::calculateVolume() const {
    return model->calculateVolume();
}

double ModelViewer::calculateProjectionArea() const {
    return model->calculateProjectionArea();
}

void ModelViewer::rotateModel(float angleX, float angleY, float angleZ) {
    model->rotateX(angleX);
    model->rotateY(angleY);
    model->rotateZ(angleZ);
    viewer->update(); // Обновляем отображение
}

void ModelViewer::translateModel(float dx, float dy, float dz) {
    model->translate(dx, dy, dz);
    viewer->update(); // Обновляем отображение
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

    // Методы для поворота и перемещения модели
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

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0)
{
    setMinimumSize(400, 400); // Устанавливаем минимальный размер окна
    setMouseTracking(true); // Включаем отслеживание движения мыши
}

void Viewer::setModel(Model *model) {
    this->model = model; // Устанавливаем модель
    update(); // Обновляем отображение
}

void Viewer::setScale(float scale) {
    this->scale = scale; // Устанавливаем масштаб
    update(); // Обновляем отображение
}

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
        if (i == selectedVertexIndex)
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
    }void Viewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos(); // Запоминаем позицию мыши

        if (model) {
            const QVector<QVector3D> &vertices = model->getVertices();
            int closestVertexIndex = -1;
            float minDistance = 10.0f; // Максимальное расстояние для выбора вершины

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

void Viewer::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // Вычисляем изменение углов поворота
        rotationX += (event->y() - lastMousePosition.y()) * 0.01;
        rotationY += (event->x() - lastMousePosition.x()) * 0.01;
        lastMousePosition = event->pos(); // Обновляем позицию мыши
        update(); // Обновляем отображение
    }
}

void Viewer::wheelEvent(QWheelEvent *event) {
    // Масштабирование модели
    if (event->angleDelta().y() > 0) {
        scale *= 1.1; // Увеличение масштаба
    } else {
        scale /= 1.1; // Уменьшение масштаба
    }
    update(); // Обновляем отображение
}




