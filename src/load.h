#include <fstream>
#include <sstream>
#include <glm/vec3.hpp>
#include "gl.h"

struct Face {
    std::vector<int> vertexIndices;
};

bool loadOBJ(const std::string& path, std::vector<glm::vec3>& out_vertices, std::vector<Face>& out_faces) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            out_vertices.push_back(vertex);
        }
        else if (type == "f") {
            Face face;
            std::string vertexIndexStr;
            while (iss >> vertexIndexStr) {
                std::istringstream vertexIndexStream(vertexIndexStr);
                int vertexIndex;
                vertexIndexStream >> vertexIndex;
                --vertexIndex; // Correct for obj format which starts at 1 not 0
                face.vertexIndices.push_back(vertexIndex);
            }
            out_faces.push_back(face);
        }
    }

    return true;
}



