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


