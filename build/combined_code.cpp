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
#include <QClipboard>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>

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
}

MainWindow::~MainWindow() {}

void MainWindow::openModel()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть модель", "", "OBJ Files (*.obj)");

    if (!filePath.isEmpty()) {
        if (modelViewer->loadModel(filePath)) {
            // Получаем данные о модели через методы ModelViewer
            QVector3D dimensions = modelViewer->getModelDimensions();
            double volume = modelViewer->calculateVolume();
            double area = modelViewer->calculateProjectionArea();

            // Обновляем заголовок окна с информацией о модели
            QString title = QString("3D Model Viewer | Модель: %1 | Размеры: %2x%3x%4 м | Объем: %5 м³ | Площадь проекции: %6 м²")
                            .arg(QFileInfo(filePath).fileName())
                            .arg(dimensions.x(), 0, 'f', 2)
                            .arg(dimensions.y(), 0, 'f', 2)
                            .arg(dimensions.z(), 0, 'f', 2)
                            .arg(volume, 0, 'f', 2)
                            .arg(area, 0, 'f', 2);
            setWindowTitle(title);
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


// File: mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> // Основной класс для создания главного окна
#include "modelviewer.h" // Подключение класса ModelViewer для работы с 3D-моделью

class MainWindow : public QMainWindow
{
    Q_OBJECT // Макрос для поддержки сигналов и слотов

public:
    // Конструктор главного окна
    MainWindow(QWidget *parent = nullptr);

    // Деструктор
    ~MainWindow();

private slots:
    // Слот для открытия модели
    void openModel();

    // Слот для сохранения текста (заголовка окна) в файл
    void saveText();

private:
    // Указатель на виджет для отображения и работы с 3D-моделью
    ModelViewer *modelViewer;
};

#endif // MAINWINDOW_H


// File: model.cpp
#include "model.h"
#include <QFile> // Для работы с файлами
#include <QTextStream> // Для чтения текстовых данных
#include <cmath> // Для математических операций

// Конструктор модели
Model::Model() {}

// Загрузка модели из файла
bool Model::load(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    // Очистка предыдущих данных
    vertices.clear();
    faces.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        // Обработка вершин
        if (parts[0] == "v") {
            float x = parts[1].toFloat() / 1000.0f; // Перевод из мм в м
            float y = parts[2].toFloat() / 1000.0f;
            float z = parts[3].toFloat() / 1000.0f;
            vertices.append(QVector3D(x, y, z));
        }
        // Обработка граней
        else if (parts[0] == "f") {
            QVector<int> face;
            for (int i = 1; i < parts.size(); ++i) {
                face.append(parts[i].split("/")[0].toInt() - 1); // Индексация с 0
            }
            faces.append(face);
        }
    }

    file.close();
    return true;
}

// Расчет объема модели
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

// Расчет площади проекции модели
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

// Получение размеров модели
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

// Получение списка вершин
const QVector<QVector3D>& Model::getVertices() const {
    return vertices;
}

// Получение списка граней
const QVector<QVector<int>>& Model::getFaces() const {
    return faces;
}

// Поворот модели вокруг оси X
void Model::rotateX(float angle) {
    float rad = qDegreesToRadians(angle); // Преобразуем угол в радианы
    for (QVector3D &vertex : vertices) {
        float y = vertex.y() * cos(rad) - vertex.z() * sin(rad);
        float z = vertex.y() * sin(rad) + vertex.z() * cos(rad);
        vertex.setY(y);
        vertex.setZ(z);
    }
}

// Поворот модели вокруг оси Y
void Model::rotateY(float angle) {
    float rad = qDegreesToRadians(angle); // Преобразуем угол в радианы
    for (QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rad) + vertex.z() * sin(rad);
        float z = -vertex.x() * sin(rad) + vertex.z() * cos(rad);
        vertex.setX(x);
        vertex.setZ(z);
    }
}

// Поворот модели вокруг оси Z
void Model::rotateZ(float angle) {
    float rad = qDegreesToRadians(angle); // Преобразуем угол в радианы
    for (QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rad) - vertex.y() * sin(rad);
        float y = vertex.x() * sin(rad) + vertex.y() * cos(rad);
        vertex.setX(x);
        vertex.setY(y);
    }
}



// File: model.h
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
        // Автоматическое масштабирование при загрузке модели
        QVector3D dimensions = model->getModelDimensions();
        float maxDimension = qMax(dimensions.x(), qMax(dimensions.y(), dimensions.z()));
        if (maxDimension > 0) {
            viewer->setScale(1.0f / maxDimension * 200); // Масштабирование для полного отображения
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
#include <QVBoxLayout>
#include <QHBoxLayout> // Добавлен для горизонтального размещения

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0),
      targetRotationX(0), targetRotationY(0), rotationSpeed(10)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);

    // Инициализация таймера для плавного вращения
    rotationTimer = new QTimer(this);
    connect(rotationTimer, &QTimer::timeout, this, &Viewer::updateRotation);
    rotationTimer->start(16); // ~60 FPS (16 мс)

    // Добавление ползунка для регулировки скорости вращения
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1, 100); // Диапазон скорости
    speedSlider->setValue(rotationSpeed);
    speedSlider->setFixedWidth(200); // Уменьшение длины ползунка
    connect(speedSlider, &QSlider::valueChanged, this, &Viewer::setRotationSpeed);

    // Размещение ползунка внизу окна
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addStretch(); // Добавляем растяжение слева
    sliderLayout->addWidget(speedSlider); // Добавляем ползунок
    sliderLayout->addStretch(); // Добавляем растяжение справа

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(); // Растяжение для размещения ползунка внизу
    mainLayout->addLayout(sliderLayout); // Добавляем ползунок внизу
    setLayout(mainLayout);
}
void Viewer::setModel(Model *model)
{
    this->model = model;
    update();
}

void Viewer::setScale(float scale)
{
    this->scale = scale;
    update();
}

void Viewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!model) {
        painter.drawText(rect(), Qt::AlignCenter, "Модель не загружена");
        return;
    }

    // Очистка экрана перед отрисовкой новой модели
    painter.fillRect(rect(), Qt::white);

    // Отрисовка модели
    const QVector<QVector3D> &vertices = model->getVertices();
    const QVector<QVector<int>> &faces = model->getFaces();

    painter.setPen(QPen(Qt::black, 2));
    for (const QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
        float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
        float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
        z = vertex.y() * sin(rotationX) + z * cos(rotationX);
        QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
        painter.drawEllipse(point, 3, 3);
    }
    painter.setPen(QPen(Qt::blue, 2));
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue;
        QPolygonF polygon;
        for (int index : face) {
            const QVector3D &vertex = vertices[index];
            float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
            float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
            float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
            z = vertex.y() * sin(rotationX) + z * cos(rotationX);

            QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
            polygon << point;
        }
        painter.drawPolygon(polygon);
    }
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos();
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // Обновляем целевые углы поворота
        targetRotationX += (event->y() - lastMousePosition.y()) * 0.5;
        targetRotationY += (event->x() - lastMousePosition.x()) * 0.5;
        lastMousePosition = event->pos();
    }
}

void Viewer::wheelEvent(QWheelEvent *event)
{
    // Масштабирование модели
    if (event->angleDelta().y() > 0) {
        scale *= 1.1; // Увеличение масштаба
    } else {
        scale /= 1.1; // Уменьшение масштаба
    }
    update(); // Обновляем отображение при изменении масштаба
}

void Viewer::updateRotation()
{
    // Плавное изменение углов поворота с учетом скорости вращения
    float deltaX = (targetRotationX - rotationX) * 0.1f;
    float deltaY = (targetRotationY - rotationY) * 0.1f;

    rotationX += deltaX * rotationSpeed / 10.0f; // Регулировка по скорости
    rotationY += deltaY * rotationSpeed / 10.0f; // Регулировка по скорости

    // Обновляем отображение
    update();
}

void Viewer::setRotationSpeed(int speed)
{
    rotationSpeed = speed; // Установка новой скорости вращения
}


// File: viewer.h
#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QTimer>
#include <QSlider> // Добавлен для регулировки скорости вращения
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

private slots:
    void updateRotation(); // Слот для обновления углов поворота
    void setRotationSpeed(int speed); // Слот для установки скорости вращения

private:
    Model *model;
    QPoint lastMousePosition;
    float rotationX;
    float rotationY;
    float scale;
    QTimer *rotationTimer; // Таймер для плавного вращения
    float targetRotationX; // Целевой угол поворота по X
    float targetRotationY; // Целевой угол поворота по Y
    int rotationSpeed; // Скорость вращения
    QSlider *speedSlider; // Ползунок для регулировки скорости
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
#include <QClipboard>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>

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
}

MainWindow::~MainWindow() {}

void MainWindow::openModel()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть модель", "", "OBJ Files (*.obj)");

    if (!filePath.isEmpty()) {
        if (modelViewer->loadModel(filePath)) {
            // Получаем данные о модели через методы ModelViewer
            QVector3D dimensions = modelViewer->getModelDimensions();
            double volume = modelViewer->calculateVolume();
            double area = modelViewer->calculateProjectionArea();

            // Обновляем заголовок окна с информацией о модели
            QString title = QString("3D Model Viewer | Модель: %1 | Размеры: %2x%3x%4 м | Объем: %5 м³ | Площадь проекции: %6 м²")
                            .arg(QFileInfo(filePath).fileName())
                            .arg(dimensions.x(), 0, 'f', 2)
                            .arg(dimensions.y(), 0, 'f', 2)
                            .arg(dimensions.z(), 0, 'f', 2)
                            .arg(volume, 0, 'f', 2)
                            .arg(area, 0, 'f', 2);
            setWindowTitle(title);
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


// File: mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> // Основной класс для создания главного окна
#include "modelviewer.h" // Подключение класса ModelViewer для работы с 3D-моделью

class MainWindow : public QMainWindow
{
    Q_OBJECT // Макрос для поддержки сигналов и слотов

public:
    // Конструктор главного окна
    MainWindow(QWidget *parent = nullptr);

    // Деструктор
    ~MainWindow();

private slots:
    // Слот для открытия модели
    void openModel();

    // Слот для сохранения текста (заголовка окна) в файл
    void saveText();

private:
    // Указатель на виджет для отображения и работы с 3D-моделью
    ModelViewer *modelViewer;
};

#endif // MAINWINDOW_H


// File: model.cpp
#include "model.h"
#include <QFile> // Для работы с файлами
#include <QTextStream> // Для чтения текстовых данных
#include <cmath> // Для математических операций

// Конструктор модели
Model::Model() {}

// Загрузка модели из файла
bool Model::load(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    // Очистка предыдущих данных
    vertices.clear();
    faces.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        // Обработка вершин
        if (parts[0] == "v") {
            float x = parts[1].toFloat() / 1000.0f; // Перевод из мм в м
            float y = parts[2].toFloat() / 1000.0f;
            float z = parts[3].toFloat() / 1000.0f;
            vertices.append(QVector3D(x, y, z));
        }
        // Обработка граней
        else if (parts[0] == "f") {
            QVector<int> face;
            for (int i = 1; i < parts.size(); ++i) {
                face.append(parts[i].split("/")[0].toInt() - 1); // Индексация с 0
            }
            faces.append(face);
        }
    }

    file.close();
    return true;
}

// Расчет объема модели
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

// Расчет площади проекции модели
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

// Получение размеров модели
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

// Получение списка вершин
const QVector<QVector3D>& Model::getVertices() const {
    return vertices;
}

// Получение списка граней
const QVector<QVector<int>>& Model::getFaces() const {
    return faces;
}

// Поворот модели вокруг оси X
void Model::rotateX(float angle) {
    float rad = qDegreesToRadians(angle); // Преобразуем угол в радианы
    for (QVector3D &vertex : vertices) {
        float y = vertex.y() * cos(rad) - vertex.z() * sin(rad);
        float z = vertex.y() * sin(rad) + vertex.z() * cos(rad);
        vertex.setY(y);
        vertex.setZ(z);
    }
}

// Поворот модели вокруг оси Y
void Model::rotateY(float angle) {
    float rad = qDegreesToRadians(angle); // Преобразуем угол в радианы
    for (QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rad) + vertex.z() * sin(rad);
        float z = -vertex.x() * sin(rad) + vertex.z() * cos(rad);
        vertex.setX(x);
        vertex.setZ(z);
    }
}

// Поворот модели вокруг оси Z
void Model::rotateZ(float angle) {
    float rad = qDegreesToRadians(angle); // Преобразуем угол в радианы
    for (QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rad) - vertex.y() * sin(rad);
        float y = vertex.x() * sin(rad) + vertex.y() * cos(rad);
        vertex.setX(x);
        vertex.setY(y);
    }
}



// File: model.h
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
        // Автоматическое масштабирование при загрузке модели
        QVector3D dimensions = model->getModelDimensions();
        float maxDimension = qMax(dimensions.x(), qMax(dimensions.y(), dimensions.z()));
        if (maxDimension > 0) {
            viewer->setScale(1.0f / maxDimension * 200); // Масштабирование для полного отображения
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
#include <QVBoxLayout>
#include <QHBoxLayout> // Добавлен для горизонтального размещения

Viewer::Viewer(QWidget *parent)
    : QWidget(parent), model(nullptr), rotationX(0), rotationY(0), scale(1.0),
      targetRotationX(0), targetRotationY(0), rotationSpeed(10)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);

    // Инициализация таймера для плавного вращения
    rotationTimer = new QTimer(this);
    connect(rotationTimer, &QTimer::timeout, this, &Viewer::updateRotation);
    rotationTimer->start(16); // ~60 FPS (16 мс)

    // Добавление ползунка для регулировки скорости вращения
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1, 100); // Диапазон скорости
    speedSlider->setValue(rotationSpeed);
    speedSlider->setFixedWidth(200); // Уменьшение длины ползунка
    connect(speedSlider, &QSlider::valueChanged, this, &Viewer::setRotationSpeed);

    // Размещение ползунка внизу окна
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addStretch(); // Добавляем растяжение слева
    sliderLayout->addWidget(speedSlider); // Добавляем ползунок
    sliderLayout->addStretch(); // Добавляем растяжение справа

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(); // Растяжение для размещения ползунка внизу
    mainLayout->addLayout(sliderLayout); // Добавляем ползунок внизу
    setLayout(mainLayout);
}
void Viewer::setModel(Model *model)
{
    this->model = model;
    update();
}

void Viewer::setScale(float scale)
{
    this->scale = scale;
    update();
}

void Viewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!model) {
        painter.drawText(rect(), Qt::AlignCenter, "Модель не загружена");
        return;
    }

    // Очистка экрана перед отрисовкой новой модели
    painter.fillRect(rect(), Qt::white);

    // Отрисовка модели
    const QVector<QVector3D> &vertices = model->getVertices();
    const QVector<QVector<int>> &faces = model->getFaces();

    painter.setPen(QPen(Qt::black, 2));
    for (const QVector3D &vertex : vertices) {
        float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
        float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
        float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
        z = vertex.y() * sin(rotationX) + z * cos(rotationX);
        QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
        painter.drawEllipse(point, 3, 3);
    }
    painter.setPen(QPen(Qt::blue, 2));
    for (const QVector<int> &face : faces) {
        if (face.size() < 3) continue;
        QPolygonF polygon;
        for (int index : face) {
            const QVector3D &vertex = vertices[index];
            float x = vertex.x() * cos(rotationY) - vertex.z() * sin(rotationY);
            float z = vertex.x() * sin(rotationY) + vertex.z() * cos(rotationY);
            float y = vertex.y() * cos(rotationX) - z * sin(rotationX);
            z = vertex.y() * sin(rotationX) + z * cos(rotationX);

            QPointF point((x * scale) + width() / 2, height() / 2 - (y * scale));
            polygon << point;
        }
        painter.drawPolygon(polygon);
    }
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos();
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // Обновляем целевые углы поворота
        targetRotationX += (event->y() - lastMousePosition.y()) * 0.5;
        targetRotationY += (event->x() - lastMousePosition.x()) * 0.5;
        lastMousePosition = event->pos();
    }
}

void Viewer::wheelEvent(QWheelEvent *event)
{
    // Масштабирование модели
    if (event->angleDelta().y() > 0) {
        scale *= 1.1; // Увеличение масштаба
    } else {
        scale /= 1.1; // Уменьшение масштаба
    }
    update(); // Обновляем отображение при изменении масштаба
}

void Viewer::updateRotation()
{
    // Плавное изменение углов поворота с учетом скорости вращения
    float deltaX = (targetRotationX - rotationX) * 0.1f;
    float deltaY = (targetRotationY - rotationY) * 0.1f;

    rotationX += deltaX * rotationSpeed / 10.0f; // Регулировка по скорости
    rotationY += deltaY * rotationSpeed / 10.0f; // Регулировка по скорости

    // Обновляем отображение
    update();
}

void Viewer::setRotationSpeed(int speed)
{
    rotationSpeed = speed; // Установка новой скорости вращения
}


// File: CMakeCXXCompilerId.cpp
/* This source file must have a .cpp extension so that all C++ compilers
   recognize the extension without flags.  Borland does not know .cxx for
   example.  */
#ifndef __cplusplus
# error "A C compiler has been selected for C++."
#endif

#if !defined(__has_include)
/* If the compiler does not have __has_include, pretend the answer is
   always no.  */
#  define __has_include(x) 0
#endif


/* Version number components: V=Version, R=Revision, P=Patch
   Version date components:   YYYY=Year, MM=Month,   DD=Day  */

#if defined(__COMO__)
# define COMPILER_ID "Comeau"
  /* __COMO_VERSION__ = VRR */
# define COMPILER_VERSION_MAJOR DEC(__COMO_VERSION__ / 100)
# define COMPILER_VERSION_MINOR DEC(__COMO_VERSION__ % 100)

#elif defined(__INTEL_COMPILER) || defined(__ICC)
# define COMPILER_ID "Intel"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# if defined(__GNUC__)
#  define SIMULATE_ID "GNU"
# endif
  /* __INTEL_COMPILER = VRP prior to 2021, and then VVVV for 2021 and later,
     except that a few beta releases use the old format with V=2021.  */
# if __INTEL_COMPILER < 2021 || __INTEL_COMPILER == 202110 || __INTEL_COMPILER == 202111
#  define COMPILER_VERSION_MAJOR DEC(__INTEL_COMPILER/100)
#  define COMPILER_VERSION_MINOR DEC(__INTEL_COMPILER/10 % 10)
#  if defined(__INTEL_COMPILER_UPDATE)
#   define COMPILER_VERSION_PATCH DEC(__INTEL_COMPILER_UPDATE)
#  else
#   define COMPILER_VERSION_PATCH DEC(__INTEL_COMPILER   % 10)
#  endif
# else
#  define COMPILER_VERSION_MAJOR DEC(__INTEL_COMPILER)
#  define COMPILER_VERSION_MINOR DEC(__INTEL_COMPILER_UPDATE)
   /* The third version component from --version is an update index,
      but no macro is provided for it.  */
#  define COMPILER_VERSION_PATCH DEC(0)
# endif
# if defined(__INTEL_COMPILER_BUILD_DATE)
   /* __INTEL_COMPILER_BUILD_DATE = YYYYMMDD */
#  define COMPILER_VERSION_TWEAK DEC(__INTEL_COMPILER_BUILD_DATE)
# endif
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif
# if defined(__GNUC__)
#  define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
# elif defined(__GNUG__)
#  define SIMULATE_VERSION_MAJOR DEC(__GNUG__)
# endif
# if defined(__GNUC_MINOR__)
#  define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
# endif
# if defined(__GNUC_PATCHLEVEL__)
#  define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
# endif

#elif (defined(__clang__) && defined(__INTEL_CLANG_COMPILER)) || defined(__INTEL_LLVM_COMPILER)
# define COMPILER_ID "IntelLLVM"
#if defined(_MSC_VER)
# define SIMULATE_ID "MSVC"
#endif
#if defined(__GNUC__)
# define SIMULATE_ID "GNU"
#endif
/* __INTEL_LLVM_COMPILER = VVVVRP prior to 2021.2.0, VVVVRRPP for 2021.2.0 and
 * later.  Look for 6 digit vs. 8 digit version number to decide encoding.
 * VVVV is no smaller than the current year when a version is released.
 */
#if __INTEL_LLVM_COMPILER < 1000000L
# define COMPILER_VERSION_MAJOR DEC(__INTEL_LLVM_COMPILER/100)
# define COMPILER_VERSION_MINOR DEC(__INTEL_LLVM_COMPILER/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__INTEL_LLVM_COMPILER    % 10)
#else
# define COMPILER_VERSION_MAJOR DEC(__INTEL_LLVM_COMPILER/10000)
# define COMPILER_VERSION_MINOR DEC(__INTEL_LLVM_COMPILER/100 % 100)
# define COMPILER_VERSION_PATCH DEC(__INTEL_LLVM_COMPILER     % 100)
#endif
#if defined(_MSC_VER)
  /* _MSC_VER = VVRR */
# define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
# define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
#endif
#if defined(__GNUC__)
# define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
#elif defined(__GNUG__)
# define SIMULATE_VERSION_MAJOR DEC(__GNUG__)
#endif
#if defined(__GNUC_MINOR__)
# define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
#endif
#if defined(__GNUC_PATCHLEVEL__)
# define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
#endif

#elif defined(__PATHCC__)
# define COMPILER_ID "PathScale"
# define COMPILER_VERSION_MAJOR DEC(__PATHCC__)
# define COMPILER_VERSION_MINOR DEC(__PATHCC_MINOR__)
# if defined(__PATHCC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__PATHCC_PATCHLEVEL__)
# endif

#elif defined(__BORLANDC__) && defined(__CODEGEARC_VERSION__)
# define COMPILER_ID "Embarcadero"
# define COMPILER_VERSION_MAJOR HEX(__CODEGEARC_VERSION__>>24 & 0x00FF)
# define COMPILER_VERSION_MINOR HEX(__CODEGEARC_VERSION__>>16 & 0x00FF)
# define COMPILER_VERSION_PATCH DEC(__CODEGEARC_VERSION__     & 0xFFFF)

#elif defined(__BORLANDC__)
# define COMPILER_ID "Borland"
  /* __BORLANDC__ = 0xVRR */
# define COMPILER_VERSION_MAJOR HEX(__BORLANDC__>>8)
# define COMPILER_VERSION_MINOR HEX(__BORLANDC__ & 0xFF)

#elif defined(__WATCOMC__) && __WATCOMC__ < 1200
# define COMPILER_ID "Watcom"
   /* __WATCOMC__ = VVRR */
# define COMPILER_VERSION_MAJOR DEC(__WATCOMC__ / 100)
# define COMPILER_VERSION_MINOR DEC((__WATCOMC__ / 10) % 10)
# if (__WATCOMC__ % 10) > 0
#  define COMPILER_VERSION_PATCH DEC(__WATCOMC__ % 10)
# endif

#elif defined(__WATCOMC__)
# define COMPILER_ID "OpenWatcom"
   /* __WATCOMC__ = VVRP + 1100 */
# define COMPILER_VERSION_MAJOR DEC((__WATCOMC__ - 1100) / 100)
# define COMPILER_VERSION_MINOR DEC((__WATCOMC__ / 10) % 10)
# if (__WATCOMC__ % 10) > 0
#  define COMPILER_VERSION_PATCH DEC(__WATCOMC__ % 10)
# endif

#elif defined(__SUNPRO_CC)
# define COMPILER_ID "SunPro"
# if __SUNPRO_CC >= 0x5100
   /* __SUNPRO_CC = 0xVRRP */
#  define COMPILER_VERSION_MAJOR HEX(__SUNPRO_CC>>12)
#  define COMPILER_VERSION_MINOR HEX(__SUNPRO_CC>>4 & 0xFF)
#  define COMPILER_VERSION_PATCH HEX(__SUNPRO_CC    & 0xF)
# else
   /* __SUNPRO_CC = 0xVRP */
#  define COMPILER_VERSION_MAJOR HEX(__SUNPRO_CC>>8)
#  define COMPILER_VERSION_MINOR HEX(__SUNPRO_CC>>4 & 0xF)
#  define COMPILER_VERSION_PATCH HEX(__SUNPRO_CC    & 0xF)
# endif

#elif defined(__HP_aCC)
# define COMPILER_ID "HP"
  /* __HP_aCC = VVRRPP */
# define COMPILER_VERSION_MAJOR DEC(__HP_aCC/10000)
# define COMPILER_VERSION_MINOR DEC(__HP_aCC/100 % 100)
# define COMPILER_VERSION_PATCH DEC(__HP_aCC     % 100)

#elif defined(__DECCXX)
# define COMPILER_ID "Compaq"
  /* __DECCXX_VER = VVRRTPPPP */
# define COMPILER_VERSION_MAJOR DEC(__DECCXX_VER/10000000)
# define COMPILER_VERSION_MINOR DEC(__DECCXX_VER/100000  % 100)
# define COMPILER_VERSION_PATCH DEC(__DECCXX_VER         % 10000)

#elif defined(__IBMCPP__) && defined(__COMPILER_VER__)
# define COMPILER_ID "zOS"
  /* __IBMCPP__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMCPP__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMCPP__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMCPP__    % 10)

#elif defined(__open_xl__) && defined(__clang__)
# define COMPILER_ID "IBMClang"
# define COMPILER_VERSION_MAJOR DEC(__open_xl_version__)
# define COMPILER_VERSION_MINOR DEC(__open_xl_release__)
# define COMPILER_VERSION_PATCH DEC(__open_xl_modification__)
# define COMPILER_VERSION_TWEAK DEC(__open_xl_ptf_fix_level__)


#elif defined(__ibmxl__) && defined(__clang__)
# define COMPILER_ID "XLClang"
# define COMPILER_VERSION_MAJOR DEC(__ibmxl_version__)
# define COMPILER_VERSION_MINOR DEC(__ibmxl_release__)
# define COMPILER_VERSION_PATCH DEC(__ibmxl_modification__)
# define COMPILER_VERSION_TWEAK DEC(__ibmxl_ptf_fix_level__)


#elif defined(__IBMCPP__) && !defined(__COMPILER_VER__) && __IBMCPP__ >= 800
# define COMPILER_ID "XL"
  /* __IBMCPP__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMCPP__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMCPP__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMCPP__    % 10)

#elif defined(__IBMCPP__) && !defined(__COMPILER_VER__) && __IBMCPP__ < 800
# define COMPILER_ID "VisualAge"
  /* __IBMCPP__ = VRP */
# define COMPILER_VERSION_MAJOR DEC(__IBMCPP__/100)
# define COMPILER_VERSION_MINOR DEC(__IBMCPP__/10 % 10)
# define COMPILER_VERSION_PATCH DEC(__IBMCPP__    % 10)

#elif defined(__NVCOMPILER)
# define COMPILER_ID "NVHPC"
# define COMPILER_VERSION_MAJOR DEC(__NVCOMPILER_MAJOR__)
# define COMPILER_VERSION_MINOR DEC(__NVCOMPILER_MINOR__)
# if defined(__NVCOMPILER_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__NVCOMPILER_PATCHLEVEL__)
# endif

#elif defined(__PGI)
# define COMPILER_ID "PGI"
# define COMPILER_VERSION_MAJOR DEC(__PGIC__)
# define COMPILER_VERSION_MINOR DEC(__PGIC_MINOR__)
# if defined(__PGIC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__PGIC_PATCHLEVEL__)
# endif

#elif defined(_CRAYC)
# define COMPILER_ID "Cray"
# define COMPILER_VERSION_MAJOR DEC(_RELEASE_MAJOR)
# define COMPILER_VERSION_MINOR DEC(_RELEASE_MINOR)

#elif defined(__TI_COMPILER_VERSION__)
# define COMPILER_ID "TI"
  /* __TI_COMPILER_VERSION__ = VVVRRRPPP */
# define COMPILER_VERSION_MAJOR DEC(__TI_COMPILER_VERSION__/1000000)
# define COMPILER_VERSION_MINOR DEC(__TI_COMPILER_VERSION__/1000   % 1000)
# define COMPILER_VERSION_PATCH DEC(__TI_COMPILER_VERSION__        % 1000)

#elif defined(__CLANG_FUJITSU)
# define COMPILER_ID "FujitsuClang"
# define COMPILER_VERSION_MAJOR DEC(__FCC_major__)
# define COMPILER_VERSION_MINOR DEC(__FCC_minor__)
# define COMPILER_VERSION_PATCH DEC(__FCC_patchlevel__)
# define COMPILER_VERSION_INTERNAL_STR __clang_version__


#elif defined(__FUJITSU)
# define COMPILER_ID "Fujitsu"
# if defined(__FCC_version__)
#   define COMPILER_VERSION __FCC_version__
# elif defined(__FCC_major__)
#   define COMPILER_VERSION_MAJOR DEC(__FCC_major__)
#   define COMPILER_VERSION_MINOR DEC(__FCC_minor__)
#   define COMPILER_VERSION_PATCH DEC(__FCC_patchlevel__)
# endif
# if defined(__fcc_version)
#   define COMPILER_VERSION_INTERNAL DEC(__fcc_version)
# elif defined(__FCC_VERSION)
#   define COMPILER_VERSION_INTERNAL DEC(__FCC_VERSION)
# endif


#elif defined(__ghs__)
# define COMPILER_ID "GHS"
/* __GHS_VERSION_NUMBER = VVVVRP */
# ifdef __GHS_VERSION_NUMBER
# define COMPILER_VERSION_MAJOR DEC(__GHS_VERSION_NUMBER / 100)
# define COMPILER_VERSION_MINOR DEC(__GHS_VERSION_NUMBER / 10 % 10)
# define COMPILER_VERSION_PATCH DEC(__GHS_VERSION_NUMBER      % 10)
# endif

#elif defined(__TASKING__)
# define COMPILER_ID "Tasking"
  # define COMPILER_VERSION_MAJOR DEC(__VERSION__/1000)
  # define COMPILER_VERSION_MINOR DEC(__VERSION__ % 100)
# define COMPILER_VERSION_INTERNAL DEC(__VERSION__)

#elif defined(__SCO_VERSION__)
# define COMPILER_ID "SCO"

#elif defined(__ARMCC_VERSION) && !defined(__clang__)
# define COMPILER_ID "ARMCC"
#if __ARMCC_VERSION >= 1000000
  /* __ARMCC_VERSION = VRRPPPP */
  # define COMPILER_VERSION_MAJOR DEC(__ARMCC_VERSION/1000000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCC_VERSION/10000 % 100)
  # define COMPILER_VERSION_PATCH DEC(__ARMCC_VERSION     % 10000)
#else
  /* __ARMCC_VERSION = VRPPPP */
  # define COMPILER_VERSION_MAJOR DEC(__ARMCC_VERSION/100000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCC_VERSION/10000 % 10)
  # define COMPILER_VERSION_PATCH DEC(__ARMCC_VERSION    % 10000)
#endif


#elif defined(__clang__) && defined(__apple_build_version__)
# define COMPILER_ID "AppleClang"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# define COMPILER_VERSION_MAJOR DEC(__clang_major__)
# define COMPILER_VERSION_MINOR DEC(__clang_minor__)
# define COMPILER_VERSION_PATCH DEC(__clang_patchlevel__)
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif
# define COMPILER_VERSION_TWEAK DEC(__apple_build_version__)

#elif defined(__clang__) && defined(__ARMCOMPILER_VERSION)
# define COMPILER_ID "ARMClang"
  # define COMPILER_VERSION_MAJOR DEC(__ARMCOMPILER_VERSION/1000000)
  # define COMPILER_VERSION_MINOR DEC(__ARMCOMPILER_VERSION/10000 % 100)
  # define COMPILER_VERSION_PATCH DEC(__ARMCOMPILER_VERSION     % 10000)
# define COMPILER_VERSION_INTERNAL DEC(__ARMCOMPILER_VERSION)

#elif defined(__clang__)
# define COMPILER_ID "Clang"
# if defined(_MSC_VER)
#  define SIMULATE_ID "MSVC"
# endif
# define COMPILER_VERSION_MAJOR DEC(__clang_major__)
# define COMPILER_VERSION_MINOR DEC(__clang_minor__)
# define COMPILER_VERSION_PATCH DEC(__clang_patchlevel__)
# if defined(_MSC_VER)
   /* _MSC_VER = VVRR */
#  define SIMULATE_VERSION_MAJOR DEC(_MSC_VER / 100)
#  define SIMULATE_VERSION_MINOR DEC(_MSC_VER % 100)
# endif

#elif defined(__LCC__) && (defined(__GNUC__) || defined(__GNUG__) || defined(__MCST__))
# define COMPILER_ID "LCC"
# define COMPILER_VERSION_MAJOR DEC(__LCC__ / 100)
# define COMPILER_VERSION_MINOR DEC(__LCC__ % 100)
# if defined(__LCC_MINOR__)
#  define COMPILER_VERSION_PATCH DEC(__LCC_MINOR__)
# endif
# if defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define SIMULATE_ID "GNU"
#  define SIMULATE_VERSION_MAJOR DEC(__GNUC__)
#  define SIMULATE_VERSION_MINOR DEC(__GNUC_MINOR__)
#  if defined(__GNUC_PATCHLEVEL__)
#   define SIMULATE_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
#  endif
# endif

#elif defined(__GNUC__) || defined(__GNUG__)
# define COMPILER_ID "GNU"
# if defined(__GNUC__)
#  define COMPILER_VERSION_MAJOR DEC(__GNUC__)
# else
#  define COMPILER_VERSION_MAJOR DEC(__GNUG__)
# endif
# if defined(__GNUC_MINOR__)
#  define COMPILER_VERSION_MINOR DEC(__GNUC_MINOR__)
# endif
# if defined(__GNUC_PATCHLEVEL__)
#  define COMPILER_VERSION_PATCH DEC(__GNUC_PATCHLEVEL__)
# endif

#elif defined(_MSC_VER)
# define COMPILER_ID "MSVC"
  /* _MSC_VER = VVRR */
# define COMPILER_VERSION_MAJOR DEC(_MSC_VER / 100)
# define COMPILER_VERSION_MINOR DEC(_MSC_VER % 100)
# if defined(_MSC_FULL_VER)
#  if _MSC_VER >= 1400
    /* _MSC_FULL_VER = VVRRPPPPP */
#   define COMPILER_VERSION_PATCH DEC(_MSC_FULL_VER % 100000)
#  else
    /* _MSC_FULL_VER = VVRRPPPP */
#   define COMPILER_VERSION_PATCH DEC(_MSC_FULL_VER % 10000)
#  endif
# endif
# if defined(_MSC_BUILD)
#  define COMPILER_VERSION_TWEAK DEC(_MSC_BUILD)
# endif

#elif defined(_ADI_COMPILER)
# define COMPILER_ID "ADSP"
#if defined(__VERSIONNUM__)
  /* __VERSIONNUM__ = 0xVVRRPPTT */
#  define COMPILER_VERSION_MAJOR DEC(__VERSIONNUM__ >> 24 & 0xFF)
#  define COMPILER_VERSION_MINOR DEC(__VERSIONNUM__ >> 16 & 0xFF)
#  define COMPILER_VERSION_PATCH DEC(__VERSIONNUM__ >> 8 & 0xFF)
#  define COMPILER_VERSION_TWEAK DEC(__VERSIONNUM__ & 0xFF)
#endif

#elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
# define COMPILER_ID "IAR"
# if defined(__VER__) && defined(__ICCARM__)
#  define COMPILER_VERSION_MAJOR DEC((__VER__) / 1000000)
#  define COMPILER_VERSION_MINOR DEC(((__VER__) / 1000) % 1000)
#  define COMPILER_VERSION_PATCH DEC((__VER__) % 1000)
#  define COMPILER_VERSION_INTERNAL DEC(__IAR_SYSTEMS_ICC__)
# elif defined(__VER__) && (defined(__ICCAVR__) || defined(__ICCRX__) || defined(__ICCRH850__) || defined(__ICCRL78__) || defined(__ICC430__) || defined(__ICCRISCV__) || defined(__ICCV850__) || defined(__ICC8051__) || defined(__ICCSTM8__))
#  define COMPILER_VERSION_MAJOR DEC((__VER__) / 100)
#  define COMPILER_VERSION_MINOR DEC((__VER__) - (((__VER__) / 100)*100))
#  define COMPILER_VERSION_PATCH DEC(__SUBVERSION__)
#  define COMPILER_VERSION_INTERNAL DEC(__IAR_SYSTEMS_ICC__)
# endif


/* These compilers are either not known or too old to define an
  identification macro.  Try to identify the platform and guess that
  it is the native compiler.  */
#elif defined(__hpux) || defined(__hpua)
# define COMPILER_ID "HP"

#else /* unknown compiler */
# define COMPILER_ID ""
#endif

/* Construct the string literal in pieces to prevent the source from
   getting matched.  Store it in a pointer rather than an array
   because some compilers will just produce instructions to fill the
   array rather than assigning a pointer to a static array.  */
char const* info_compiler = "INFO" ":" "compiler[" COMPILER_ID "]";
#ifdef SIMULATE_ID
char const* info_simulate = "INFO" ":" "simulate[" SIMULATE_ID "]";
#endif

#ifdef __QNXNTO__
char const* qnxnto = "INFO" ":" "qnxnto[]";
#endif

#if defined(__CRAYXT_COMPUTE_LINUX_TARGET)
char const *info_cray = "INFO" ":" "compiler_wrapper[CrayPrgEnv]";
#endif

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

/* Identify known platforms by name.  */
#if defined(__linux) || defined(__linux__) || defined(linux)
# define PLATFORM_ID "Linux"

#elif defined(__MSYS__)
# define PLATFORM_ID "MSYS"

#elif defined(__CYGWIN__)
# define PLATFORM_ID "Cygwin"

#elif defined(__MINGW32__)
# define PLATFORM_ID "MinGW"

#elif defined(__APPLE__)
# define PLATFORM_ID "Darwin"

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
# define PLATFORM_ID "Windows"

#elif defined(__FreeBSD__) || defined(__FreeBSD)
# define PLATFORM_ID "FreeBSD"

#elif defined(__NetBSD__) || defined(__NetBSD)
# define PLATFORM_ID "NetBSD"

#elif defined(__OpenBSD__) || defined(__OPENBSD)
# define PLATFORM_ID "OpenBSD"

#elif defined(__sun) || defined(sun)
# define PLATFORM_ID "SunOS"

#elif defined(_AIX) || defined(__AIX) || defined(__AIX__) || defined(__aix) || defined(__aix__)
# define PLATFORM_ID "AIX"

#elif defined(__hpux) || defined(__hpux__)
# define PLATFORM_ID "HP-UX"

#elif defined(__HAIKU__)
# define PLATFORM_ID "Haiku"

#elif defined(__BeOS) || defined(__BEOS__) || defined(_BEOS)
# define PLATFORM_ID "BeOS"

#elif defined(__QNX__) || defined(__QNXNTO__)
# define PLATFORM_ID "QNX"

#elif defined(__tru64) || defined(_tru64) || defined(__TRU64__)
# define PLATFORM_ID "Tru64"

#elif defined(__riscos) || defined(__riscos__)
# define PLATFORM_ID "RISCos"

#elif defined(__sinix) || defined(__sinix__) || defined(__SINIX__)
# define PLATFORM_ID "SINIX"

#elif defined(__UNIX_SV__)
# define PLATFORM_ID "UNIX_SV"

#elif defined(__bsdos__)
# define PLATFORM_ID "BSDOS"

#elif defined(_MPRAS) || defined(MPRAS)
# define PLATFORM_ID "MP-RAS"

#elif defined(__osf) || defined(__osf__)
# define PLATFORM_ID "OSF1"

#elif defined(_SCO_SV) || defined(SCO_SV) || defined(sco_sv)
# define PLATFORM_ID "SCO_SV"

#elif defined(__ultrix) || defined(__ultrix__) || defined(_ULTRIX)
# define PLATFORM_ID "ULTRIX"

#elif defined(__XENIX__) || defined(_XENIX) || defined(XENIX)
# define PLATFORM_ID "Xenix"

#elif defined(__WATCOMC__)
# if defined(__LINUX__)
#  define PLATFORM_ID "Linux"

# elif defined(__DOS__)
#  define PLATFORM_ID "DOS"

# elif defined(__OS2__)
#  define PLATFORM_ID "OS2"

# elif defined(__WINDOWS__)
#  define PLATFORM_ID "Windows3x"

# elif defined(__VXWORKS__)
#  define PLATFORM_ID "VxWorks"

# else /* unknown platform */
#  define PLATFORM_ID
# endif

#elif defined(__INTEGRITY)
# if defined(INT_178B)
#  define PLATFORM_ID "Integrity178"

# else /* regular Integrity */
#  define PLATFORM_ID "Integrity"
# endif

# elif defined(_ADI_COMPILER)
#  define PLATFORM_ID "ADSP"

#else /* unknown platform */
# define PLATFORM_ID

#endif

/* For windows compilers MSVC and Intel we can determine
   the architecture of the compiler being used.  This is because
   the compilers do not have flags that can change the architecture,
   but rather depend on which compiler is being used
*/
#if defined(_WIN32) && defined(_MSC_VER)
# if defined(_M_IA64)
#  define ARCHITECTURE_ID "IA64"

# elif defined(_M_ARM64EC)
#  define ARCHITECTURE_ID "ARM64EC"

# elif defined(_M_X64) || defined(_M_AMD64)
#  define ARCHITECTURE_ID "x64"

# elif defined(_M_IX86)
#  define ARCHITECTURE_ID "X86"

# elif defined(_M_ARM64)
#  define ARCHITECTURE_ID "ARM64"

# elif defined(_M_ARM)
#  if _M_ARM == 4
#   define ARCHITECTURE_ID "ARMV4I"
#  elif _M_ARM == 5
#   define ARCHITECTURE_ID "ARMV5I"
#  else
#   define ARCHITECTURE_ID "ARMV" STRINGIFY(_M_ARM)
#  endif

# elif defined(_M_MIPS)
#  define ARCHITECTURE_ID "MIPS"

# elif defined(_M_SH)
#  define ARCHITECTURE_ID "SHx"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__WATCOMC__)
# if defined(_M_I86)
#  define ARCHITECTURE_ID "I86"

# elif defined(_M_IX86)
#  define ARCHITECTURE_ID "X86"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
# if defined(__ICCARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__ICCRX__)
#  define ARCHITECTURE_ID "RX"

# elif defined(__ICCRH850__)
#  define ARCHITECTURE_ID "RH850"

# elif defined(__ICCRL78__)
#  define ARCHITECTURE_ID "RL78"

# elif defined(__ICCRISCV__)
#  define ARCHITECTURE_ID "RISCV"

# elif defined(__ICCAVR__)
#  define ARCHITECTURE_ID "AVR"

# elif defined(__ICC430__)
#  define ARCHITECTURE_ID "MSP430"

# elif defined(__ICCV850__)
#  define ARCHITECTURE_ID "V850"

# elif defined(__ICC8051__)
#  define ARCHITECTURE_ID "8051"

# elif defined(__ICCSTM8__)
#  define ARCHITECTURE_ID "STM8"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__ghs__)
# if defined(__PPC64__)
#  define ARCHITECTURE_ID "PPC64"

# elif defined(__ppc__)
#  define ARCHITECTURE_ID "PPC"

# elif defined(__ARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__x86_64__)
#  define ARCHITECTURE_ID "x64"

# elif defined(__i386__)
#  define ARCHITECTURE_ID "X86"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

#elif defined(__TI_COMPILER_VERSION__)
# if defined(__TI_ARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__MSP430__)
#  define ARCHITECTURE_ID "MSP430"

# elif defined(__TMS320C28XX__)
#  define ARCHITECTURE_ID "TMS320C28x"

# elif defined(__TMS320C6X__) || defined(_TMS320C6X)
#  define ARCHITECTURE_ID "TMS320C6x"

# else /* unknown architecture */
#  define ARCHITECTURE_ID ""
# endif

# elif defined(__ADSPSHARC__)
#  define ARCHITECTURE_ID "SHARC"

# elif defined(__ADSPBLACKFIN__)
#  define ARCHITECTURE_ID "Blackfin"

#elif defined(__TASKING__)

# if defined(__CTC__) || defined(__CPTC__)
#  define ARCHITECTURE_ID "TriCore"

# elif defined(__CMCS__)
#  define ARCHITECTURE_ID "MCS"

# elif defined(__CARM__)
#  define ARCHITECTURE_ID "ARM"

# elif defined(__CARC__)
#  define ARCHITECTURE_ID "ARC"

# elif defined(__C51__)
#  define ARCHITECTURE_ID "8051"

# elif defined(__CPCP__)
#  define ARCHITECTURE_ID "PCP"

# else
#  define ARCHITECTURE_ID ""
# endif

#else
#  define ARCHITECTURE_ID
#endif

/* Convert integer to decimal digit literals.  */
#define DEC(n)                   \
  ('0' + (((n) / 10000000)%10)), \
  ('0' + (((n) / 1000000)%10)),  \
  ('0' + (((n) / 100000)%10)),   \
  ('0' + (((n) / 10000)%10)),    \
  ('0' + (((n) / 1000)%10)),     \
  ('0' + (((n) / 100)%10)),      \
  ('0' + (((n) / 10)%10)),       \
  ('0' +  ((n) % 10))

/* Convert integer to hex digit literals.  */
#define HEX(n)             \
  ('0' + ((n)>>28 & 0xF)), \
  ('0' + ((n)>>24 & 0xF)), \
  ('0' + ((n)>>20 & 0xF)), \
  ('0' + ((n)>>16 & 0xF)), \
  ('0' + ((n)>>12 & 0xF)), \
  ('0' + ((n)>>8  & 0xF)), \
  ('0' + ((n)>>4  & 0xF)), \
  ('0' + ((n)     & 0xF))

/* Construct a string literal encoding the version number. */
#ifdef COMPILER_VERSION
char const* info_version = "INFO" ":" "compiler_version[" COMPILER_VERSION "]";

/* Construct a string literal encoding the version number components. */
#elif defined(COMPILER_VERSION_MAJOR)
char const info_version[] = {
  'I', 'N', 'F', 'O', ':',
  'c','o','m','p','i','l','e','r','_','v','e','r','s','i','o','n','[',
  COMPILER_VERSION_MAJOR,
# ifdef COMPILER_VERSION_MINOR
  '.', COMPILER_VERSION_MINOR,
#  ifdef COMPILER_VERSION_PATCH
   '.', COMPILER_VERSION_PATCH,
#   ifdef COMPILER_VERSION_TWEAK
    '.', COMPILER_VERSION_TWEAK,
#   endif
#  endif
# endif
  ']','\0'};
#endif

/* Construct a string literal encoding the internal version number. */
#ifdef COMPILER_VERSION_INTERNAL
char const info_version_internal[] = {
  'I', 'N', 'F', 'O', ':',
  'c','o','m','p','i','l','e','r','_','v','e','r','s','i','o','n','_',
  'i','n','t','e','r','n','a','l','[',
  COMPILER_VERSION_INTERNAL,']','\0'};
#elif defined(COMPILER_VERSION_INTERNAL_STR)
char const* info_version_internal = "INFO" ":" "compiler_version_internal[" COMPILER_VERSION_INTERNAL_STR "]";
#endif

/* Construct a string literal encoding the version number components. */
#ifdef SIMULATE_VERSION_MAJOR
char const info_simulate_version[] = {
  'I', 'N', 'F', 'O', ':',
  's','i','m','u','l','a','t','e','_','v','e','r','s','i','o','n','[',
  SIMULATE_VERSION_MAJOR,
# ifdef SIMULATE_VERSION_MINOR
  '.', SIMULATE_VERSION_MINOR,
#  ifdef SIMULATE_VERSION_PATCH
   '.', SIMULATE_VERSION_PATCH,
#   ifdef SIMULATE_VERSION_TWEAK
    '.', SIMULATE_VERSION_TWEAK,
#   endif
#  endif
# endif
  ']','\0'};
#endif

/* Construct the string literal in pieces to prevent the source from
   getting matched.  Store it in a pointer rather than an array
   because some compilers will just produce instructions to fill the
   array rather than assigning a pointer to a static array.  */
char const* info_platform = "INFO" ":" "platform[" PLATFORM_ID "]";
char const* info_arch = "INFO" ":" "arch[" ARCHITECTURE_ID "]";



#if defined(__INTEL_COMPILER) && defined(_MSVC_LANG) && _MSVC_LANG < 201403L
#  if defined(__INTEL_CXX11_MODE__)
#    if defined(__cpp_aggregate_nsdmi)
#      define CXX_STD 201402L
#    else
#      define CXX_STD 201103L
#    endif
#  else
#    define CXX_STD 199711L
#  endif
#elif defined(_MSC_VER) && defined(_MSVC_LANG)
#  define CXX_STD _MSVC_LANG
#else
#  define CXX_STD __cplusplus
#endif

const char* info_language_standard_default = "INFO" ":" "standard_default["
#if CXX_STD > 202002L
  "23"
#elif CXX_STD > 201703L
  "20"
#elif CXX_STD >= 201703L
  "17"
#elif CXX_STD >= 201402L
  "14"
#elif CXX_STD >= 201103L
  "11"
#else
  "98"
#endif
"]";

const char* info_language_extensions_default = "INFO" ":" "extensions_default["
#if (defined(__clang__) || defined(__GNUC__) || defined(__xlC__) ||           \
     defined(__TI_COMPILER_VERSION__)) &&                                     \
  !defined(__STRICT_ANSI__)
  "ON"
#else
  "OFF"
#endif
"]";

/*--------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
  int require = 0;
  require += info_compiler[argc];
  require += info_platform[argc];
  require += info_arch[argc];
#ifdef COMPILER_VERSION_MAJOR
  require += info_version[argc];
#endif
#ifdef COMPILER_VERSION_INTERNAL
  require += info_version_internal[argc];
#endif
#ifdef SIMULATE_ID
  require += info_simulate[argc];
#endif
#ifdef SIMULATE_VERSION_MAJOR
  require += info_simulate_version[argc];
#endif
#if defined(__CRAYXT_COMPUTE_LINUX_TARGET)
  require += info_cray[argc];
#endif
  require += info_language_standard_default[argc];
  require += info_language_extensions_default[argc];
  (void)argv;
  return require;
}


