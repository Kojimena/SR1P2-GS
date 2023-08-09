#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Fragment {
public:
    glm::ivec2 position; // Coordenadas X e Y del pixel (en espacio de pantalla)

    // Otros atributos interpolados pueden ser agregados aquí
    // Por ejemplo:
    glm::vec4 color; // Color del fragmento (RGBA)
    glm::vec2 texCoords; // Coordenadas de textura
    glm::vec3 normal; // Normal del fragmento (en caso de que se use para iluminación, por ejemplo)

    // Puedes agregar más atributos según lo necesites

    // Constructor por defecto
    Fragment() = default;

    // Constructor con posición
    Fragment(const glm::ivec2& pos) : position(pos) {}

    // Constructor con posición y color
    Fragment(const glm::ivec2& pos, const glm::vec4& col) : position(pos), color(col) {}

};
