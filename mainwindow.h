#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modelviewer.h"
#include <QTextEdit> // Добавляем для текстового поля

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openModel();
    void saveText();
    void rotateModel();
    void translateModel();
    void updateWindowTitle();
    void updateVertexInfo(int index, const QVector3D &vertex); // Новый слот для обновления информации о вершине

private:
    ModelViewer *modelViewer;
    QTextEdit *infoPanel; // Добавляем текстовое поле для информации
    QString modelFileName; // Переменная для хранения имени файла модели
};

#endif // MAINWINDOW_H
