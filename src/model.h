#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <glm/glm.hpp> // Используем glm для работы с векторами и матрицами

class Model {
public:
    Model(const std::string& filename);
    void load();
    void draw();
    float calculateVolume();
    float calculateProjectedArea();

private:
    std::vector<glm::vec3> vertices; // Вершины модели
    std::vector<glm::vec3> normals;  // Нормали
    std::vector<glm::vec2> uvs;      // Координаты текстур
    std::string filename;             // Имя файла модели
};

#endif // MODEL_H
