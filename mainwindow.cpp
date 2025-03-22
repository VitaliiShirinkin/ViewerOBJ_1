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
            QVector3D dimensions = modelViewer->getModelDimensions();
            double volume = modelViewer->calculateVolume();
            double area = modelViewer->calculateProjectionArea();

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
        dialog.close();
    });

    dialog.exec();
}

