#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Конструктор
Model::Model(const std::string& filename) : filename(filename) {}

// Загрузка модели из файла
void Model::load() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        // Здесь можно добавить обработку других типов (например, нормали, текстуры)
    }
}

// Функция отрисовки модели
void Model::draw() {
    // Реализуйте отрисовку с использованием OpenGL
}

// Функция для расчета объема модели
float Model::calculateVolume() {
    float volume = 0.0f;
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glm::vec3 v0 = vertices[i];
        glm::vec3 v1 = vertices[i + 1];
        glm::vec3 v2 = vertices[i + 2];
        volume += glm::dot(v0, glm::cross(v1, v2)) / 6.0f;
    }
    return std::abs(volume);
}

// Функция для расчета площади проекции модели
float Model::calculateProjectedArea() {
    float area = 0.0f;
    for (size_t i = 0; i < vertices.size(); i += 3) {
        glm::vec3 v0 = vertices[i];
        glm::vec3 v1 = vertices[i + 1];
        glm::vec3 v2 = vertices[i + 2];
        area += std::abs((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y)) / 2.0f;
    }
    return area;
}
