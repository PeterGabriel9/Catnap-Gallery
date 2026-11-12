#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "sprite_renderer.h" 

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
    void Render();   //deseneaza 

    void DrawSword(glm::vec2 pos, float rotAngle);  
    void DrawEnemy(); 

    bool CheckCollision(glm::vec3 pos1, glm::vec2 size1, glm::vec3 pos2, glm::vec2 size2);      
    void OnScroll(double yoffSet);

private:
    GLuint programID;
    GLuint texture;
    SpriteRenderer* Renderer;      // Pointerul catre SpriteRenderer

    GLuint whiteTexture;

    // Datele pisicii
    glm::vec3 squarePos;
    glm::vec2 squareSize;          
    float speed;

    //datele SABIE
    GLuint swordTexture;
    glm::vec3 swordPos;
    glm::vec2 swordSize;
    float swordRotationAngle;//unghiul la care e rotita
    bool isAttacking;
    float attackTimer; //cat timp a trecut de la atac ca sa putem anima frumos
    float swordSpeed;
    float swordMaxDistance;
    glm::vec3 swordStartPos; // sa stim unde se intoarce sabia
    //HELD-> SABIA LA PISICA -> SPace => Thrown -> MaxDstance => RETURNING => HELD
    enum SwordState { HELD, THROWN, RETURNING }; //3 stari
    SwordState swordState;

    //INAMIC
    glm:: vec3 enemyPos;
    glm::vec2 enemySize;
    float enemyMaxHealth;
    float enemyHealth ;
    bool enemyAlive;
    float enemyTimer;


};