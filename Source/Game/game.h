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

// Which scene the player is currently in
enum Scene {
    SCENE_MUSEUM,
    SCENE_PAINTING
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
    GLuint catSwordTexture;   // Meow holding the sword (shown when swordState == HELD)
    SpriteRenderer* Renderer;      // Pointerul catre SpriteRenderer

    GLuint whiteTexture;
    GLuint bgTexture; //id for backgrounf picture
    GLuint museumTexture; // museum scene background (with the C.A.T.Scan machine)

    // Datele pisicii
    glm::vec3 catPos;
    glm::vec2 catSize;          
    float speed;
    float velocityY; // vertical speed
    float gravity; // gravoty force that is briging her back down
    float jumpStrength;
    bool catIsGrounded;
    float groundLevel;
    float facingDir;   // +1 = facing right, -1 = facing left

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
    float swordDir;          // direction the sword was thrown: +1 right, -1 left
    //HELD-> SABIA LA PISICA -> SPace => Thrown -> MaxDstance => RETURNING => HELD
    enum SwordState { HELD, THROWN, RETURNING , GROUND}; //4 stari , la GROUND daca nu prindem sabia, pica pe jos si mergem sa o luam
    SwordState swordState;

    //INAMIC
    glm:: vec3 enemyPos;
    glm::vec2 enemySize;
    float enemyMaxHealth;
    float enemyHealth ;
    bool enemyAlive;
    float enemyTimer;

    // BOSS (mounted commander) - patrols left<->right in the painting
    GLuint bossTexture;
    glm::vec3 bossPos;
    glm::vec2 bossSize;
    float bossSpeed;
    float bossDir;            // +1 moving right, -1 moving left
    float bossMaxHealth;
    float bossHealth;
    bool bossAlive;
    glm::vec3 lancePos;       // lance hitbox (attached in front of the boss)
    glm::vec2 lanceSize;
    void DrawBoss();

    // SCENE / STORY
    Scene currentScene;              // which scene we are in (museum / painting)
    glm::vec3 machinePos;            // invisible hitbox for the C.A.T.Scan machine (drawn in the background art)
    glm::vec2 machineSize;

    // PUZZLE PIECE (collectible fragment in the painting)
    GLuint puzzleTexture;
    glm::vec3 puzzlePos;             // its base/anchor position
    glm::vec2 puzzleSize;
    bool puzzleCollected;            // true once the cat picks it up
    float puzzleTimer;               // grows over time, drives the bob + spin


};