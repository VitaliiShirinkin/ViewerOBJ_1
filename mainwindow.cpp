#include "mainwindow.h"          // Подключение заголовочного файла MainWindow
#include <QMenuBar>              // Для создания меню в главном окне
#include <QMenu>                 // Для создания пунктов меню
#include <QAction>               // Для создания действий в меню
#include <QFileDialog>           // Для выбора файла через диалоговое окно
#include <QMessageBox>           // Для отображения сообщений об ошибках

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), modelViewer(new ModelViewer(this))  // Инициализация ModelViewer
{
    setWindowTitle("3D Model Viewer");  // Установка заголовка окна
    setCentralWidget(modelViewer);      // Установка ModelViewer в качестве центрального виджета

    // Создание меню "File"
    QMenu *fileMenu = menuBar()->addMenu("File");  // Добавление меню в строку меню

    // Создание действия "Open" в меню "File"
    QAction *openAction = fileMenu->addAction("Open");  // Добавление действия "Open"
    connect(openAction, &QAction::triggered, this, &MainWindow::openModel);  // Подключение слота openModel
}

MainWindow::~MainWindow() {}  // Деструктор (пока пустой)

// Слот для открытия модели
void MainWindow::openModel()
{
    // Открытие диалогового окна для выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Open Model", "", "OBJ Files (*.obj)");

    if (!filePath.isEmpty()) {  // Если файл выбран
        modelViewer->loadModel(filePath);  // Загрузка модели в ModelViewer
    } else {
        QMessageBox::warning(this, "Warning", "No file selected.");  // Сообщение, если файл не выбран
    }
}
