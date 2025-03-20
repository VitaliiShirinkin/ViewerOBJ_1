#include "modelviewer.h"  // Подключение заголовочного файла ModelViewer
#include <QVBoxLayout>    // Для управления компоновкой виджетов
#include <QFileDialog>    // Для выбора файла через диалоговое окно
#include <QMessageBox>    // Для отображения сообщений об ошибках

ModelViewer::ModelViewer(QWidget *parent)
    : QWidget(parent), model(new Model()), viewer(new Viewer(this))  // Инициализация модели и Viewer
{
    QVBoxLayout *layout = new QVBoxLayout(this);  // Создание вертикальной компоновки
    layout->addWidget(viewer);  // Добавление Viewer в компоновку
    setLayout(layout);  // Установка компоновки для текущего виджета
}

// Метод для загрузки модели из файла
void ModelViewer::loadModel(const QString &filePath)
{
    if (model->load(filePath)) {  // Попытка загрузить модель
        viewer->setModel(model);  // Установка модели в Viewer для отображения
    } else {
        QMessageBox::critical(this, "Error", "Failed to load the model.");  // Сообщение об ошибке
    }
}
