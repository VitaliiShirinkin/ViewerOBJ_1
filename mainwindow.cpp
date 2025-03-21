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
