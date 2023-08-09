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
    glm::mat4 rotation = glm::rotate(glm::mat4(0.9f), rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));

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

std::vector<Fragment> triangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    std::vector<Fragment> fragments;

    // Convertir las coordenadas del modelo a coordenadas de pantalla
    int x1 = (v1.x + 1) * SCREEN_WIDTH / 2;
    int y1 = (v1.y + 1) * SCREEN_HEIGHT / 2;
    int x2 = (v2.x + 1) * SCREEN_WIDTH / 2;
    int y2 = (v2.y + 1) * SCREEN_HEIGHT / 2;
    int x3 = (v3.x + 1) * SCREEN_WIDTH / 2;
    int y3 = (v3.y + 1) * SCREEN_HEIGHT / 2;

    // Por ahora, simplemente agregamos los tres vértices del triángulo como fragmentos
    fragments.push_back(Fragment(glm::ivec2(x1, y1)));
    fragments.push_back(Fragment(glm::ivec2(x2, y2)));
    fragments.push_back(Fragment(glm::ivec2(x3, y3)));

    return fragments;
}



std::vector<glm::vec3> modelVertices;




// Primitive Assembly function
std::vector<std::vector<glm::vec3>> primitiveAssembly(const std::vector<glm::vec3>& transformedVertices) {
    std::vector<std::vector<glm::vec3>> groupedVertices;
    for (size_t i = 0; i < transformedVertices.size(); i += 3) {
        std::vector<glm::vec3> triangle = {transformedVertices[i], transformedVertices[i+1], transformedVertices[i+2]};
        groupedVertices.push_back(triangle);
    }
    return groupedVertices;
}

Color fragmentShader(const Fragment& fragment) {
    // Ejemplo: Asignar un color constante a cada fragmento
    Color fragColor(255, 0, 0, 255); // Color rojo con opacidad completa

    // Puedes modificar esta función para implementar un sombreado más complejo
    // basado en los atributos del fragmento (por ejemplo, profundidad, normales interpoladas, coordenadas de textura, etc.)

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

    // Scale
    viewport = glm::scale(viewport, glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.5f));

    // Translate
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
    float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
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

    // 2. Primitive Assembly
    auto groupedVertices = primitiveAssembly(transformedVertices);

    // 3. Rasterization
    auto fragments = rasterize(groupedVertices);


    // 4. Fragment Shader
    for (const auto& fragment : fragments) {
        Color pixelColor = fragmentShader(fragment);
        point(fragment.position.x, fragment.position.y); // Dibuja el píxel en la posición del fragmento
    }

    for (size_t i = 0; i < vertices.size(); i += 3) {
        drawTriangle(vertices[i], vertices[i+1], vertices[i+2]);
    }

    for (size_t i = 0; i < vertices.size(); i += 3) {
        drawTriangle(vertices[i], vertices[i+1], vertices[i+2]);
    }
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