#pragma once
#include "GL/glew.h"
#include "glm/glm.hpp"

class SpriteRenderer {
public:
    // Constructor: creates the shader program ID and the reausable quad
    SpriteRenderer(GLuint shader);
    ~SpriteRenderer(); //destructor

    // Draw any sprite anywhere onthe screen: texture, where, how big, rotation(degrees), color
    void DrawSprite(GLuint texture,
                    glm::vec2 position,                       
                    glm::vec2 size = glm::vec2(0.5f, 0.5f),     
                    float rotate = 0.0f,                         
                    glm::vec3 color = glm::vec3(1.0f));         

private:
    GLuint shaderProgram;  // shader ID
    GLuint quadVAO;        // Vertex Array Object ID of the reausable quad
    void initRenderData(); //creates the quad geometry once
}; 