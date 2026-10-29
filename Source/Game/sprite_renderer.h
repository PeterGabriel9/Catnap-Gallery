 #pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

class SpriteRenderer {
public:
    // Primeste shader-ul si creeaza patratul refolosibil
    SpriteRenderer(GLuint shader);
    ~SpriteRenderer();

    // Deseneaza un sprite oriunde pe ecran
    void DrawSprite(GLuint texture,
                    glm::vec2 position,                          // unde (centrul sprite-ului)
                    glm::vec2 size = glm::vec2(0.5f, 0.5f),     // cat de mare
                    float rotate = 0.0f,                         // rotatie in grade
                    glm::vec3 color = glm::vec3(1.0f));          // culoare/nuanta (alb = original)

private:
    GLuint shaderProgram;  // ID-ul shader-ului
    GLuint quadVAO;        // Patratul refolosibil (un singur VAO pentru ORICE sprite!)

    // Creeaza patratul (VAO/VBO) o singura data
    void initRenderData();
}; 