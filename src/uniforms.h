#pragma once

#include <glm/glm.hpp>

struct Uniforms {
    glm::mat4 model;      // Matriz de modelo: transforma vértices desde el espacio del objeto al espacio mundial
    glm::mat4 view;       // Matriz de vista: transforma vértices desde el espacio mundial al espacio de la cámara
    glm::mat4 projection; // Matriz de proyección: transforma vértices desde el espacio de la cámara al espacio de proyección
    glm::mat4 viewport;   // Matriz de viewport: transforma vértices desde el espacio de proyección al espacio de pantalla

    // Constructor por defecto
    Uniforms() = default;

    // Constructor con todas las matrices
    Uniforms(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p, const glm::mat4& vp)
            : model(m), view(v), projection(p), viewport(vp) {}
};
