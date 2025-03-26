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
#include <QSplitter> // Добавляем для разделения окна

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), modelViewer(new ModelViewer(this))
{
    setWindowTitle("Viewer Obj");

    QSplitter *splitter = new QSplitter(this);
    setCentralWidget(splitter);

    splitter->addWidget(modelViewer);

    infoPanel = new QTextEdit(this);
    infoPanel->setReadOnly(true);
    infoPanel->setFixedWidth(this->width() * 0.2);
    splitter->addWidget(infoPanel);

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

    // Используем метод getViewer для подключения сигнала
    connect(modelViewer->getViewer(), &Viewer::vertexSelected, this, &MainWindow::updateVertexInfo);
}

MainWindow::~MainWindow() {}

void MainWindow::openModel()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Открыть модель", "", "OBJ Files (*.obj)");

    if (!filePath.isEmpty()) {
        if (modelViewer->loadModel(filePath)) {
            // Сохраняем имя файла модели
            QFileInfo fileInfo(filePath);
            modelFileName = fileInfo.fileName(); // Получаем только имя файла (без пути)
            updateWindowTitle();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить модель.");
        }
    } else {
        QMessageBox::warning(this, "Предупреждение", "Файл не выбран.");
    }
}

void MainWindow::updateVertexInfo(int index, const QVector3D &vertex) {
    // Формируем текст с информацией о вершине
    QString vertexInfo = QString("Выделенная вершина:\n"
                                 "Индекс: %1\n"
                                 "Координаты: (%2, %3, %4)")
                            .arg(index)
                            .arg(vertex.x(), 0, 'f', 2)
                            .arg(vertex.y(), 0, 'f', 2)
                            .arg(vertex.z(), 0, 'f', 2);

    // Обновляем текстовое поле
    infoPanel->append(vertexInfo); // Добавляем информацию о вершине
}

void MainWindow::updateWindowTitle()
{
    // Получаем размеры модели
    QVector3D dimensions = modelViewer->getModelDimensions();
    // Вычисляем объем модели
    double volume = modelViewer->calculateVolume();
    // Вычисляем площадь проекции модели
    double area = modelViewer->calculateProjectionArea();

    // Формируем текст для панели информации
    QString infoText = QString("Название модели: %1\n"
                               "Размеры: %2x%3x%4 м\n"
                               "Объем: %5 м³\n"
                               "Площадь проекции: %6 м²")
                          .arg(modelFileName) // Используем имя файла модели
                          .arg(dimensions.x(), 0, 'f', 2)
                          .arg(dimensions.y(), 0, 'f', 2)
                          .arg(dimensions.z(), 0, 'f', 2)
                          .arg(volume, 0, 'f', 2)
                          .arg(area, 0, 'f', 2);

    // Обновляем текстовое поле
    infoPanel->setText(infoText);
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
        updateWindowTitle();
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
        updateWindowTitle();
        dialog.close();
    });

    dialog.exec();
}


