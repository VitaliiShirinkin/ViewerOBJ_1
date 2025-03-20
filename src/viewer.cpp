#include "Viewer.h"
#include <QOpenGLFunctions>

// Конструктор
Viewer::Viewer(QWidget* parent) : QOpenGLWidget(parent), model(nullptr) {
    model = new Model("path/to/your/model.obj"); // Укажите путь к вашей модели
    model->load();
}

// Деструктор
Viewer::~Viewer() {
    delete model;
}

// Инициализация OpenGL
void Viewer::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST); // Включаем тест глубины для правильной отрисовки
}

// Изменение размера окна
void Viewer::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

// Отрисовка сцены
void Viewer::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очищаем буферы
    model->draw(); // Отрисовка модели

    // Выводим объем и площадь проекции в консоль
    float volume = model->calculateVolume();
    float area = model->calculateProjectedArea();
    std::cout << "Объем модели: " << volume << std::endl;
    std::cout << "Площадь проекции модели: " << area << std::endl;
}
