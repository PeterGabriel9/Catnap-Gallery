#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "sprite_renderer.h" // Il includem pe noul nostru renderer!

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

    void Init();                    // Incarca shaderele, textura si porneste renderer-ul
    void ProcessInput(float dt);    // Misca pisica cu WASD si opreste la margini
    void Update(float dt);          // Logica jocului
    void Render();                  // Deseneaza pisica printr-un simplu DrawSprite!
    void OnScroll(double yoffSet);

private:
    GLuint programID;
    GLuint texture;
    SpriteRenderer* Renderer;       // Pointerul catre SpriteRenderer!

    // Datele pisicii tale:
    glm::vec3 squarePos;
    glm::vec2 squareSize;           // Latimea si inaltimea (inlocuieste zoomScale)
    float speed;
};