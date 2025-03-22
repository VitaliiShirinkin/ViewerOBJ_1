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

private:
    ModelViewer *modelViewer;
};

#endif // MAINWINDOW_H
