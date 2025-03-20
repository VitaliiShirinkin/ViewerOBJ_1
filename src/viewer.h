#ifndef VIEWER_H
#define VIEWER_H

#include <QOpenGLWidget>
#include "Model.h"

class Viewer : public QOpenGLWidget {
    Q_OBJECT

public:
    Viewer(QWidget* parent = nullptr);
    ~Viewer();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    Model* model; // Указатель на модель
};

#endif // VIEWER_H
