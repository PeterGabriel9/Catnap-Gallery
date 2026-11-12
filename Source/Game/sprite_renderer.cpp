#include "sprite_renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//We store the shader ID and create the reausable quad 
SpriteRenderer::SpriteRenderer(GLuint shader) {
    this->shaderProgram = shader;
    this->initRenderData(); 
}

//When thw game shuts down we clean: delete VAO
SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
}


//BUid a single square on the GPU that every sprite will use
void SpriteRenderer::initRenderData() {
    unsigned int VBO;

    // square definition
    // position x, y and texture : u,v
    float vertices[] = { 
        -0.5f, -0.5f,          0.0f, 0.0f, // Triangle 1: Left-Down
         0.5f, -0.5f,          1.0f, 0.0f, //              Right-Down
         0.5f,  0.5f,          1.0f, 1.0f, //              Right-Up
        -0.5f, -0.5f,          0.0f, 0.0f, // Triangle2 2: Left-Down
         0.5f,  0.5f,          1.0f, 1.0f, //              Right-UP
        -0.5f,  0.5f,          0.0f, 1.0f  //              Left-Down
    };

    // 1. VAO and VBO generation
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

       // 2. Upload the vertex data to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //3. Activate the VAO to record the configuration
    glBindVertexArray(this->quadVAO);
    
    // 4. Giving the data layouy : 4 floats 2 -> x,y and 2 -> u,v
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // 5.unbind: cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// DRAW SPRITE : is called every frame for every obejct 
void SpriteRenderer::DrawSprite(GLuint texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {

    // 1. Activate the shader
    glUseProgram(this->shaderProgram);

   //transformation: translate, scale, rotate
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), rotate,  glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
  
    //result right to left multiplication
    glm::mat4 model = T * R * S;

    // 3. send matrix and color to shader
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(this->shaderProgram, "spriteColor"), color.r, color.g, color.b);
 
    // 4. activating texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
   
    // 5. drawing
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}