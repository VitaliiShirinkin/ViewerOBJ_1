#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include <QWidget>          // Базовый класс для всех виджетов в Qt
#include "model.h"          // Подключение заголовочного файла модели
#include "viewer.h"         // Подключение заголовочного файла Viewer

class ModelViewer : public QWidget
{
    Q_OBJECT  // Макрос для поддержки сигналов и слотов

public:
    ModelViewer(QWidget *parent = nullptr);  // Конструктор класса ModelViewer
    void loadModel(const QString &filePath);  // Метод для загрузки модели из файла

private:
    Model *model;  // Указатель на объект модели
    Viewer *viewer;  // Указатель на объект Viewer для отображения модели
};

#endif // MODELVIEWER_H
