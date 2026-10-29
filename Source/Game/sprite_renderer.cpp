#include "sprite_renderer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// ----------------------------------------------------------------------
// CONSTRUCTOR: Salveaza shaderul si configureaza patratul in GPU
// ----------------------------------------------------------------------
SpriteRenderer::SpriteRenderer(GLuint shader) {
    this->shaderProgram = shader;
    this->initRenderData();
}

// ----------------------------------------------------------------------
// DESTRUCTOR: Curata memoria din GPU cand se inchide jocul
// ----------------------------------------------------------------------
SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
}


// ----------------------------------------------------------------------
// initRenderData: Creeaza UN SINGUR patrat refolosibil in GPU
// ----------------------------------------------------------------------
void SpriteRenderer::initRenderData() {
    unsigned int VBO;
    // Definim un patrat centrat in (0, 0), de la -0.5 la +0.5
    // Fiecare linie: Pozitie X, Y si Coordonate Textura U, V
    float vertices[] = { 
        // Pozitie (X, Y)      // Textura (U, V)
        -0.5f, -0.5f,          0.0f, 0.0f, // Triunghiul 1: Stanga-Jos
         0.5f, -0.5f,          1.0f, 0.0f, //              Dreapta-Jos
         0.5f,  0.5f,          1.0f, 1.0f, //              Dreapta-Sus
        -0.5f, -0.5f,          0.0f, 0.0f, // Triunghiul 2: Stanga-Jos
         0.5f,  0.5f,          1.0f, 1.0f, //              Dreapta-Sus
        -0.5f,  0.5f,          0.0f, 1.0f  //              Stanga-Sus
    };

    // 1. Generam VAO si VBO
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    // 2. Trimitem datele (vertices) in memoria placii video (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. Activam VAO pentru a memora configuratia
    glBindVertexArray(this->quadVAO);
    
    // 4. Ii spunem lui OpenGL cum sunt asezate datele:
    // Avem 4 float-uri per punct (2 pentru X,Y si 2 pentru U,V)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // 5. Deconectam bufferele (curatenie)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// ----------------------------------------------------------------------
// DrawSprite: Deseneaza textura la pozitia, marimea si rotatia dorita
// ----------------------------------------------------------------------
void SpriteRenderer::DrawSprite(GLuint texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {

    // 1. Activam shader-ul
    glUseProgram(this->shaderProgram);


    // A. Translatie: Muta patratul la pozitia dorita
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));


    // B. Rotatie: Roteste in jurul propriului sau centru (0, 0)
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));

    // C. Scalare: Mareste/Micsoreaza patratul la dimensiunea "size"
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
 // Exact formula matematica:

    glm::mat4 model = T * R * S;

    // 3. Trimitem matricea calculata si culoarea la shader
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(this->shaderProgram, "spriteColor"), color.r, color.g, color.b);
 
    // 4. Activam textura (pisica)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
   
    // 5. Desenam patratul (cele 6 puncte din VAO)
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}