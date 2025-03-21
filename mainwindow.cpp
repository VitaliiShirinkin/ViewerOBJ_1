#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), modelViewer(new ModelViewer(this))
{
    setWindowTitle("3D Model Viewer");
    setCentralWidget(modelViewer);

    QMenu *fileMenu = menuBar()->addMenu("Файл");
    QAction *openAction = fileMenu->addAction("Открыть");
    connect(openAction, &QAction::triggered, this, &MainWindow::openModel);
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
            QString title = QString("3D Model Viewer | Размеры: %1x%2x%3 м | Объем: %4 м³ | Площадь проекции: %5 м²")
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
