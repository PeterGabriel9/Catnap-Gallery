/* game.h e header
 TOTATE DECLARATIILE: CE EXISTA: cclass names, method names, variable naes
 
 
 => in game.cpp avem DEFINITIILE = logica efectiva*/

#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
    public:
    GameState State;
    bool Keys[1024];
    
    unsigned int Width, Height;
    Game(unsigned int width, unsigned int height);
    ~Game();
    void Init();                    // setez totul o data la incept: shadere, vertez data, VAO,VBO,IBO, texture , tot ce se intampla in main inainte de while loop
    void ProcessInput(float dt);    // verific ce taste apas si reactioneaza
    void Update(float dt);          // calculeaza tot ce se schimba frame-to-frame ex: transform matrix glm::translate ..  
    void Render();                  // dezeneaza 
    void OnScroll(double yoffSet);

    private: //OPenGl obj - se creeaza o data in Init() si nu se schimba
    GLuint vao, vbo, ibo;
    GLuint programID;
    GLuint texture;

    //Game obj state - datele care descriu obiectul(patratul) - se schimba frame-to-frame
    glm::vec3 squarePos;
    float limit;
    float zoomScale;
    float speed;
    glm::mat4 trans;
};
