#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <ctime>
#include <glm/ext/matrix_transform.hpp>
#include "gl.h"
#include "load.h"

float rotationAngle = 0.0f;


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


std::vector<glm::vec3> modelVertices;


void render() {
    for (size_t i = 0; i < modelVertices.size(); i += 3) {
        drawTriangle(modelVertices[i], modelVertices[i+1], modelVertices[i+2]);
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
    //print the information of the obj file
    for (int i = 0; i < vertices.size(); i++) {
        std::cout << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << std::endl;
    }

    for (int i = 0; i < faces.size(); i++) {
        std::cout << "f " << faces[i].vertexIndices[0] << " " << faces[i].vertexIndices[1] << " " << faces[i].vertexIndices[2] << std::endl;
    }


    modelVertices = setupVertexArray(vertices, faces);

    bool running = true;

    // Clear the screen once before starting the loop
    clear();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Clear the screen in each iteration
        clear();

        rotationAngle += 0.01f; // Incrementa el ángulo de rotación

        render();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
