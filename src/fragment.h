#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Fragment {
public:
    glm::ivec2 position; // Coordenadas X e Y del pixel (en espacio de pantalla)

    glm::vec4 color; // Color del fragmento (RGBA)
    glm::vec2 texCoords; // Coordenadas de textura
    glm::vec3 normal; // Normal del fragmento (en caso de que se use para iluminación, por ejemplo)


    Fragment() = default;

    Fragment(const glm::ivec2& pos) : position(pos) {}

    Fragment(const glm::ivec2& pos, const glm::vec4& col) : position(pos), color(col) {}

};
