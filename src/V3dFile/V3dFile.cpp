#include <iostream>
#include <fstream>
#include <queue>
#include <memory>

#include "V3dFile.h"

#include "xstream.h"

V3dFile::V3dFile(const std::string& fileName) {
    std::ifstream file(fileName);
    std::string str;

    std::queue<std::string> lines;

    while (std::getline(file, str)) {
        lines.push(str);
    }

    versionNumber = std::stoul(lines.front());
    lines.pop();
    doublePrecisionFlag = std::stoul(lines.front());
    lines.pop();

    while (!lines.empty()) {
        UINT objectType = std::stoul(lines.front());
        lines.pop();

        switch (objectType) {
        default:
            std::cout << "UNKNOWN TYPE: " << objectType << std::endl;
            break;

        case ObjectTypes::TRIANGLE:
            std::unique_ptr<V3dStraightTriangle> object = std::make_unique<V3dStraightTriangle>();

            object->vertices[0].x = std::stod(lines.front());
            lines.pop();
            object->vertices[0].y = std::stod(lines.front());
            lines.pop();
            object->vertices[0].z = std::stod(lines.front());
            lines.pop();

            object->vertices[1].x = std::stod(lines.front());
            lines.pop();
            object->vertices[1].y = std::stod(lines.front());
            lines.pop();
            object->vertices[1].z = std::stod(lines.front());
            lines.pop();

            object->vertices[2].x = std::stod(lines.front());
            lines.pop();
            object->vertices[2].y = std::stod(lines.front());
            lines.pop();
            object->vertices[2].z = std::stod(lines.front());
            lines.pop();

            object->centerIndex = std::stoul(lines.front());
            lines.pop();

            object->materialIndex = std::stoul(lines.front());
            lines.pop();

            m_Objects.push_back(std::move(object));
            break;
        };
    }

    std::vector<float> vertices;

    for (auto& object : m_Objects) {
        std::vector<float> vert = object->getVertices();

        for (auto& value : vert) {
            vertices.push_back(value);
        }
    }

    this->vertices = vertices;
}
