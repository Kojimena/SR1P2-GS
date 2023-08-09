#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <ctime>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>  // Para glm::lookAt
#include "gl.h"
#include "load.h"
#include "uniforms.h"
#include "fragment.h"

float rotationAngle = 0.0f;

// Estructura de la cámara
struct Camera {
    glm::vec3 cameraPosition;
    glm::vec3 targetPosition;
    glm::vec3 upVector;
};

// Función para crear una matriz de vista
glm::mat4 createViewMatrix(const Camera& camera) {
    return glm::lookAt(camera.cameraPosition, camera.targetPosition, camera.upVector);
}

void drawTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    // Crea una matriz de rotación
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));

    // Aplica la rotación a los vértices
    glm::vec4 v1_rotated = rotation * glm::vec4(v1, 1.0f);
    glm::vec4 v2_rotated = rotation * glm::vec4(v2, 1.0f);
    glm::vec4 v3_rotated = rotation * glm::vec4(v3, 1.0f);

    float scale = 0.1; // Adjust this value as needed
    glm::vec3 translation(0, 0, 0); // Adjust these values as needed
    glm::vec3 v1_transformed = glm::vec3(v1_rotated) * scale + translation;
    glm::vec3 v2_transformed = glm::vec3(v2_rotated) * scale + translation;
    glm::vec3 v3_transformed = glm::vec3(v3_rotated) * scale + translation;

    // Transform model coordinates to screen coordinates
    int x1 = (v1_transformed.x + 1) * SCREEN_WIDTH / 2;
    int y1 = (v1_transformed.y + 1) * SCREEN_HEIGHT / 2;
    int x2 = (v2_transformed.x + 1) * SCREEN_WIDTH / 2;
    int y2 = (v2_transformed.y + 1) * SCREEN_HEIGHT / 2;
    int x3 = (v3_transformed.x + 1) * SCREEN_WIDTH / 2;
    int y3 = (v3_transformed.y + 1) * SCREEN_HEIGHT / 2;

    SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);


    // Draw the first side of the triangle
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    // Draw the second side of the triangle
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    // Draw the third side of the triangle
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}



// Función para calcular la coordenada baricéntrica
glm::vec3 barycentricCoordinate(const glm::vec2& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    glm::vec3 v0 = b - a, v1 = c - a, v2 = glm::vec3(p.x, p.y, 0) - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    glm::vec3 w;
    w.y = (d11 * d20 - d01 * d21) / denom;
    w.z = (d00 * d21 - d01 * d20) / denom;
    w.x = 1.0f - w.y - w.z;
    return w;
}

std::vector<Fragment> triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    std::vector<Fragment> fragments;

    // Encuentra el bounding box del triángulo
    float minX = std::min({v0.x, v1.x, v2.x});
    float maxX = std::max({v0.x, v1.x, v2.x});
    float minY = std::min({v0.y, v1.y, v2.y});
    float maxY = std::max({v0.y, v1.y, v2.y});

    // Para cada píxel dentro del bounding box
    for (float x = minX; x <= maxX; x++) {
        for (float y = minY; y <= maxY; y++) {
            // Calcula la barycentric coordinate para ver si el píxel está dentro del triángulo
            glm::vec3 w = barycentricCoordinate(glm::vec2(x, y), v0, v1, v2);

            // Si todas las coordenadas baricéntricas están en el rango [0, 1], el punto está dentro del triángulo
            if (w.x >= 0 && w.x <= 1 && w.y >= 0 && w.y <= 1 && w.z >= 0 && w.z <= 1) {
                Fragment frag;
                frag.position.x = x;
                frag.position.y = y;
                fragments.push_back(frag);
            }
        }
    }

    return fragments;
}


std::vector<glm::vec3> modelVertices;




std::vector<std::vector<glm::vec3>> primitiveAssembly(const std::vector<glm::vec3>& transformedVertices) {
    std::vector<std::vector<glm::vec3>> assembled;

    for (size_t i = 0; i < transformedVertices.size(); i += 3) {
        assembled.push_back({ transformedVertices[i], transformedVertices[i + 1], transformedVertices[i + 2] });
    }

    return assembled;
}


Color fragmentShader(const Fragment& fragment) {
    Color fragColor(255, 0, 0, 255); // Red color with full opacity
    return fragColor;
}


// Function to create a model matrix based on translation, rotation, and scale
glm::mat4 createModelMatrix(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale) {
    // Start with an identity matrix
    glm::mat4 model = glm::mat4(1.0f);

    // Apply translation
    model = glm::translate(model, translation);

    // Apply rotation on X, Y, and Z axis
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    // Apply scale
    model = glm::scale(model, scale);

    return model;
}

glm::mat4 createViewportMatrix() {
    glm::mat4 viewport = glm::mat4(1.0f);
    viewport = glm::scale(viewport, glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.5f));
    viewport = glm::translate(viewport, glm::vec3(1.0f, 1.0f, 0.5f));

    return viewport;
}


// Vertex Shader function
glm::vec3 vertexShader(const glm::vec3& vertex, const Uniforms& uniforms) {
    // Apply transformations to the input vertex using the matrices from the uniforms
    glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex, 1.0f);

    // Perspective divide
    glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

    // Apply the viewport transform
    glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

    // Return the transformed vertex as a vec3
    return glm::vec3(screenVertex);
}


std::vector<Fragment> rasterize(const std::vector<std::vector<glm::vec3>>& assembledVertices) {
    std::vector<Fragment> fragments;

    for (const std::vector<glm::vec3>& triangleVertices : assembledVertices) {
        std::vector<Fragment> triangleFragments = triangle(triangleVertices[0], triangleVertices[1], triangleVertices[2]);
        fragments.insert(fragments.end(), triangleFragments.begin(), triangleFragments.end());
    }

    return fragments;
}


// Función para crear una matriz de proyección
glm::mat4 createProjectionMatrix() {
    float fovInDegrees = 45.0f;
    float aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
    float nearClip = 0.1f;
    float farClip = 100.0f;

    return glm::perspective(glm::radians(fovInDegrees), aspectRatio, nearClip, farClip);
}



void render(const std::vector<glm::vec3>& vertices, const Uniforms& uniforms) {
    // 1. Vertex Shader
    std::vector<glm::vec3> transformedVertices;
    for (const auto& vertex : vertices) {
        transformedVertices.push_back(vertexShader(vertex, uniforms));
    }

    std::cout << "transformedVertices: " << transformedVertices.size() << std::endl;

    // 2. Primitive Assembly
    auto groupedVertices = primitiveAssembly(transformedVertices);

    std::cout << "groupedVertices: " << groupedVertices.size() << std::endl;

    // 3. Rasterization
    auto fragments = rasterize(groupedVertices);

    std::cout << "fragments: " << fragments.size() << std::endl;


    // 4. Fragment Shader
    for (const auto& fragment : fragments) {
        Color pixelColor = fragmentShader(fragment);
        SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a);  // <-- Añade esta línea
        point(fragment.position.x, fragment.position.y); // Dibuja el píxel en la posición del fragmento
    }

    std::cout << "fragments: " << fragments.size() << std::endl;


    for (size_t i = 0; i < vertices.size(); i += 3) {
        drawTriangle(vertices[i], vertices[i+1], vertices[i+2]);
    }
    SDL_GetError();

    std::cout << "vertices: " << vertices.size() << std::endl;
}

std::vector<glm::vec3> setupVertexArray(const std::vector<glm::vec3>& vertices, const std::vector<Face>& faces)
{
    std::vector<glm::vec3> vertexArray;

    // For each face
    for (const auto& face : faces)
    {
        // Get the first vertex of the face
        glm::vec3 firstVertex = vertices[face.vertexIndices[0] - 1]; // Subtract 1 from the index

        // For each additional vertex in the face
        for (size_t i = 2; i < face.vertexIndices.size(); ++i)
        {
            // Get the two vertices
            glm::vec3 vertex1 = vertices[face.vertexIndices[i - 1] - 1]; // Subtract 1 from the index
            glm::vec3 vertex2 = vertices[face.vertexIndices[i] - 1]; // Subtract 1 from the index

            // Add the vertices to the vertex array
            vertexArray.push_back(firstVertex);
            vertexArray.push_back(vertex1);
            vertexArray.push_back(vertex2);
        }
    }

    return vertexArray;
}

int main(int argc, char** argv) {
    init();
    std::vector<glm::vec3> vertices;
    std::vector<Face> faces;
    if (!loadOBJ("/Users/jime/Downloads/navejime.obj", vertices, faces)) {
        std::cerr << "Failed to load model" << std::endl;
        return 1;
    }

    std::cout << "Loaded " << vertices.size() << " vertices and " << faces.size() << " faces." << std::endl;
    for (int i = 0; i < vertices.size(); i++) {
        std::cout << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
    }

    for (int i = 0; i < faces.size(); i++) {
        std::cout << "f " << faces[i].vertexIndices[0] << " " << faces[i].vertexIndices[1] << " " << faces[i].vertexIndices[2] << std::endl;
    }

    modelVertices = setupVertexArray(vertices, faces);

    // Inicialización de la cámara
    Camera camera;
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);  // Posición de la cámara
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);  // Punto al que mira la cámara
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);        // Vector hacia arriba

    Uniforms uniforms;
    uniforms.model = glm::mat4(1.0f);                     // Matriz de modelo
    uniforms.view = createViewMatrix(camera);             // Genera la matriz de vista usando la cámara
    uniforms.projection = createProjectionMatrix();       // Genera la matriz de proyección

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        clear();
        rotationAngle += 0.01f;

        uniforms.viewport = createViewportMatrix();  // Establece la matriz de viewport en los uniforms
        render(modelVertices, uniforms); // Pasamos los vértices y los uniforms a la función render

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}