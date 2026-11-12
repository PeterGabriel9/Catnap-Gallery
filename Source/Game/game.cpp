#include "game.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../stb_image.h"
#include "../Shaders/Utils/shaderLoader.hpp"
#include <iostream>
#include <cstring>

Game::Game(unsigned int width, unsigned int height) {
    Width = width;
    Height = height;
    State = GAME_ACTIVE;

    //CAT info
    squarePos = glm::vec3(0.0f, 0.0f, 0.0f);
    squareSize = glm::vec2(0.5f, 0.5f);
    speed = 0.5f;

    swordPos = glm::vec3(squarePos.x + squareSize.x* 0.5f, squarePos.y, 0.0f);
    swordSize = glm::vec2(0.1f, 0.3f);
    swordSpeed = 0.7f;
    swordMaxDistance = 0.8f;
    swordRotationAngle = 45.0f;
    swordState = HELD;

    //ENEMY info
    enemyPos = glm::vec3(0.7f, 0.0f, 0.0f);       
    enemySize = glm::vec2(0.25f, 0.35f);
    enemyMaxHealth = 100.0f;
    enemyHealth = 100.0f;
    enemyTimer = 0.0f;
    enemyAlive = true;


    Renderer = nullptr;
    memset(Keys, false, sizeof(Keys));
}

bool Game::CheckCollision(glm::vec3 pos1, glm::vec2 size1, glm::vec3 pos2, glm::vec2 size2) {
    // we check collisionof the edges on Ox
    bool collisionX = pos1.x + size1.x / 2.0f >= pos2.x - size2.x / 2.0f &&
                      pos2.x + size2.x / 2.0f >= pos1.x - size1.x / 2.0f;
    
    // Cwe check collision of the edges on Oy
    bool collisionY = pos1.y + size1.y / 2.0f >= pos2.y - size2.y / 2.0f &&
                      pos2.y + size2.y / 2.0f >= pos1.y - size1.y / 2.0f;
    
    // collision happens if both casees are happening
    return collisionX && collisionY;
}

void Game::Init() {
    // Load shaders
    programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");

    Renderer = new SpriteRenderer(programID);

    // cat.png load
    stbi_set_flip_vertically_on_load(true);
    int w, h, nrChannels;
    unsigned char* data = stbi_load("cat.png", &w, &h, &nrChannels, 4);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Imagine incarcata cu succes! Dimensiuni: " << w << "x" << h << std::endl;
    } else {
        std::cout << "EROARE CRITICA: Imaginea nu a fost gasita!" << std::endl;
    }
    stbi_image_free(data);


    unsigned char* swordData = stbi_load("sword.png", &w, &h, &nrChannels, 4);
    glGenTextures(1, &swordTexture);
    glBindTexture(GL_TEXTURE_2D, swordTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (swordData) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, swordData);
    glGenerateMipmap(GL_TEXTURE_2D);
    std::cout << "Sword loaded! " << w << "x" << h << std::endl;
    } 
    else {
    std::cout << "EROARE: sword.png nu a fost gasita!" << std::endl;
    }
    stbi_image_free(swordData);


    glGenTextures(1, &whiteTexture);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    unsigned char whitePixel[] = { 255, 255, 255, 255 }; // RGBA alb
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);




}

void Game::ProcessInput(float dt) {
    // Limita exacta pana la marginea ecranului:
    // (1.0 minus jumatate din dimensiunea patratului!)
    float limitX = 1.0f - (squareSize.x / 2.0f);
    float limitY = 1.0f - (squareSize.y / 2.0f);

    // Tastele A, D, W, S
    if (Keys[GLFW_KEY_A]) {
        squarePos.x -= speed * dt;
        swordPos = glm::vec3(squarePos.x + squareSize.x* 0.5f, squarePos.y, 0.0f);
    }
    if (Keys[GLFW_KEY_D]) {
        squarePos.x += speed * dt;
        swordPos = glm::vec3(squarePos.x + squareSize.x* 0.5f, squarePos.y, 0.0f);
    }
    if (Keys[GLFW_KEY_W]) {
        squarePos.y += speed * dt;
        swordPos = glm::vec3(squarePos.x + squareSize.x* 0.5f, squarePos.y, 0.0f);
    }
    if (Keys[GLFW_KEY_S]) {
        squarePos.y -= speed * dt;
        swordPos = glm::vec3(squarePos.x + squareSize.x* 0.5f, squarePos.y, 0.0f);
    }


    //SWORD MOVEMENT
    // the sword is thrown only if it is in state HELD and we press SPACE
    if(Keys[GLFW_KEY_SPACE] && swordState == HELD ){
        swordState= THROWN;
        swordStartPos  = squarePos;

        swordPos = squarePos;
    }

    // dont go outside screen OX
    if (squarePos.x > limitX) {
        squarePos.x= limitX;
    }
    if (squarePos.x < -limitX) {
        squarePos.x = -limitX;
    }

    // Sdont go outside scrren OY
    if (squarePos.y > limitY) {
        squarePos.y = limitY;
    }
    if (squarePos.y < -limitY) {
        squarePos.y = -limitY;
    }
}

void Game::Update(float dt) {
    
    if(swordState == HELD) {
        swordPos = squarePos;
        swordRotationAngle = 45.0f;

} else if(swordState == THROWN) {
    swordPos.x += swordSpeed * dt;
    swordRotationAngle += 1080.0f * dt;

    if(enemyAlive && CheckCollision(swordPos, swordSize, enemyPos, enemySize)) {
        enemyHealth -=34.0f;
        std::cout << "Inamic lovit! Viata: " << enemyHealth << std::endl;

            // sword goes back to player if it hits smth
            swordState = RETURNING;


         if (enemyHealth <= 0.0f) {
             enemyAlive = false;
            std::cout << "Inamicul a fost distrus!" << std::endl;
            }
    }
    //comgin back is it achives its maximum distance limit
    if(swordPos.x >= swordStartPos.x + swordMaxDistance) {
        swordState = RETURNING;
    }

} else if(swordState == RETURNING) {
    swordPos.x -= swordSpeed*dt;
    swordRotationAngle += 1080.0f *dt;

    if (swordPos.x <= squarePos.x) {
        swordState = HELD;
    }
}

}

void Game::DrawEnemy() {
    if (!enemyAlive) return;

    // Folosim enemyPos direct ca vec3
    glm::vec3 bodyPos = enemyPos;

    // 1. Corpul (Dreptunghi rosu inchis)
    Renderer->DrawSprite(whiteTexture, glm::vec2(bodyPos), enemySize, 0.0f, glm::vec3(0.6f, 0.1f, 0.1f));

    // 2. Capul (Patrat rosu deschis, pozitionat mai sus de corp)
    glm::vec2 headSize = glm::vec2(0.2f, 0.2f);
    glm::vec3 headPos = glm::vec3(bodyPos.x, bodyPos.y + enemySize.y / 2.0f + headSize.y / 2.0f, 0.0f);
    Renderer->DrawSprite(whiteTexture, glm::vec2(headPos), headSize, 0.0f, glm::vec3(0.8f, 0.2f, 0.2f));

    // 3. Ochiul stang (Patrat mic negru, decalat la stanga pe cap)
    glm::vec2 eyeSize = glm::vec2(0.04f, 0.04f);
    glm::vec3 leftEyePos = glm::vec3(headPos.x - 0.05f, headPos.y + 0.02f, 0.0f);
    Renderer->DrawSprite(whiteTexture, glm::vec2(leftEyePos), eyeSize, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    // 4. Ochiul drept (Patrat mic negru, decalat la dreapta pe cap)
    glm::vec3 rightEyePos = glm::vec3(headPos.x + 0.05f, headPos.y + 0.02f, 0.0f);
    Renderer->DrawSprite(whiteTexture, glm::vec2(rightEyePos), eyeSize, 0.0f, glm::vec3(0.0f, 0.0f, 0.0f));
}



void Game::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

   
    // DOAR O SINGURA LINIE PENTRU A DESENA!
    // Parametri: textura, pozitia (X, Y), marimea, rotatia (0 grade), culoarea (alb)
   
    Renderer->DrawSprite(texture, glm::vec2(squarePos), squareSize, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
   
    glm::vec2 drawSwordPos = glm::vec2(swordPos);
    if (swordState == HELD) {
    drawSwordPos.x += squareSize.x * 0.5f;  // offset in dreapta doar cand e tinuta
   }
    
    Renderer->DrawSprite(swordTexture, drawSwordPos, swordSize, swordRotationAngle);
    DrawEnemy();
    
}

void Game::OnScroll(double yoffset) {
    // Zoom in / Zoom out cand invarti de rotita mouse-ului
    squareSize += static_cast<float>(yoffset) * 0.05f;
    if (squareSize.x < 0.1f) squareSize = glm::vec2(0.1f, 0.1f);
    if (squareSize.x > 1.5f) squareSize = glm::vec2(1.5f, 1.5f);
}

Game::~Game() {
    delete Renderer;
    glDeleteTextures(1, &texture);
    glDeleteProgram(programID);
}