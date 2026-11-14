#include "game.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../stb_image.h"
#include "../Shaders/Utils/shaderLoader.hpp"
#include <iostream>
#include <cstring>
#include <cmath>

Game::Game(unsigned int width, unsigned int height) {
    Width = width;
    Height = height;
    State = GAME_ACTIVE;

    //CAT info
    groundLevel = -0.55;
    catPos = glm::vec3(-0.5f, groundLevel, 0.0f);
    catSize = glm::vec2(0.5f, 0.5f);
    speed = 0.5f;
    velocityY = 0.0f;
    gravity = -4.0f;            // forța care trage în jos (cu minus)
    jumpStrength = 2.8f;        // puterea saltului în sus (marita: sare mai sus)
    catIsGrounded = true;          // la început stă pe pământ
    facingDir = 1.0f;              // starts facing right
    

    swordPos = glm::vec3(catPos.x + catSize.x* 0.5f, catPos.y, 0.0f);
    swordSize = glm::vec2(0.2f, 0.4f);
    swordSpeed = 0.7f;
    swordMaxDistance = 0.8f;
    swordRotationAngle = 45.0f;
    swordState = HELD;
    swordDir = 1.0f;   // default throw direction

    //ENEMY info
    enemyPos = glm::vec3(0.7f, 0.0f, 0.0f);       
    enemySize = glm::vec2(0.25f, 0.35f);
    enemyMaxHealth = 100.0f;
    enemyHealth = 100.0f;
    enemyTimer = 0.0f;
    enemyAlive = true;

    // BOSS info (mounted commander in the painting)
    bossPos = glm::vec3(0.5f, groundLevel + 0.15f, 0.0f);  // sits a bit above the floor (he's tall on a horse)
    bossSize = glm::vec2(0.85f, 0.85f);
    bossSpeed = 0.3f;
    bossDir = -1.0f;            // starts moving left
    bossMaxHealth = 100.0f;
    bossHealth = 100.0f;
    bossAlive = true;
    lanceSize = glm::vec2(0.25f, 0.1f);   // lance hitbox in front of him

    // SCENE / STORY info
    currentScene = SCENE_MUSEUM;              // start in the museum
    machinePos = glm::vec3(-0.68f, -0.35f, 0.0f);  // over the painted C.A.T.Scan (left side, on the desk)
    machineSize = glm::vec2(0.3f, 0.4f);

    // PUZZLE PIECE info (in the painting scene)
    puzzlePos = glm::vec3(0.4f, 0.1f, 0.0f);  // floating in the battlefield
    puzzleSize = glm::vec2(0.18f, 0.18f);
    puzzleCollected = false;
    puzzleTimer = 0.0f;


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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // cat.png load
    stbi_set_flip_vertically_on_load(true);
    int w, h, nrChannels;
    unsigned char* data = stbi_load("assets/Cat_idle_first_frame.png", &w, &h, &nrChannels, 4);

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


    // cat-with-sword image (shown when the sword is HELD)
    unsigned char* catSwordData = stbi_load("assets/Cat_idle_with_sword_first_frame.png", &w, &h, &nrChannels, 4);
    glGenTextures(1, &catSwordTexture);
    glBindTexture(GL_TEXTURE_2D, catSwordTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (catSwordData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, catSwordData);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Cat-with-sword loaded! " << w << "x" << h << std::endl;
    } else {
        std::cout << "EROARE: Cat_idle_with_sword nu a fost gasita!" << std::endl;
    }
    stbi_image_free(catSwordData);


    unsigned char* swordData = stbi_load("assets/sword-Photoroom.png", &w, &h, &nrChannels, 4);
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


     unsigned char* bgData = stbi_load("assets/background_paiting_battle_1.jpeg", &w, &h, &nrChannels, 4);
    glGenTextures(1, &bgTexture);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (bgData) {
        // Trimitem pixelii in memoria placii video (VBO/Texture memory)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bgData);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Background incarcat cu succes! " << w << "x" << h << std::endl;
    } else {
        std::cout << "EROARE CRITICA: background.png nu a fost gasita!" << std::endl;
    }
    
    // Eliberam memoria RAM (pixelii sunt salvati acum in GPU)
    stbi_image_free(bgData);


    // museum scene background (contains the painted C.A.T.Scan machine)
    unsigned char* museumData = stbi_load("assets/lab_background.jpg", &w, &h, &nrChannels, 4);
    glGenTextures(1, &museumTexture);
    glBindTexture(GL_TEXTURE_2D, museumTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (museumData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, museumData);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Museum background loaded! " << w << "x" << h << std::endl;
    } else {
        std::cout << "EROARE: lab_background.jpg nu a fost gasita!" << std::endl;
    }
    stbi_image_free(museumData);


    // puzzle piece (collectible fragment)
    unsigned char* puzzleData = stbi_load("assets/puzzle-piece.png", &w, &h, &nrChannels, 4);
    glGenTextures(1, &puzzleTexture);
    glBindTexture(GL_TEXTURE_2D, puzzleTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (puzzleData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, puzzleData);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Puzzle piece loaded! " << w << "x" << h << std::endl;
    } else {
        std::cout << "EROARE: puzzle-piece.png nu a fost gasita!" << std::endl;
    }
    stbi_image_free(puzzleData);


    // boss (mounted commander)
    unsigned char* bossData = stbi_load("assets/boss_fight-Photoroom.png", &w, &h, &nrChannels, 4);
    glGenTextures(1, &bossTexture);
    glBindTexture(GL_TEXTURE_2D, bossTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (bossData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bossData);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Boss loaded! " << w << "x" << h << std::endl;
    } else {
        std::cout << "EROARE: boss_fight-Photoroom.png nu a fost gasita!" << std::endl;
    }
    stbi_image_free(bossData);


}

void Game::ProcessInput(float dt) {
    // Limita exacta pana la marginea ecranului:
    // (1.0 minus jumatate din dimensiunea patratului!)
    float limitX = 1.0f - (catSize.x / 2.0f);
    float limitY = 1.0f - (catSize.y / 2.0f);

    // Tastele A, D, W, S
    if (Keys[GLFW_KEY_A]) {
        catPos.x -= speed * dt;
        facingDir = -1.0f;   // now facing left
    }
    if (Keys[GLFW_KEY_D]) {
        catPos.x += speed * dt;
        facingDir = 1.0f;    // now facing right
    }
    if (Keys[GLFW_KEY_SPACE] && catIsGrounded) {
        velocityY = jumpStrength;  // îi dă impulsul în sus
        catIsGrounded = false;
    }

    // STORY: press E near the C.A.T.Scan machine to jump into the painting
    if (Keys[GLFW_KEY_E] && currentScene == SCENE_MUSEUM) {
        // "near" = cat and machine overlap (proximity check)
        if (CheckCollision(catPos, catSize, machinePos, machineSize)) {
            currentScene = SCENE_PAINTING;   // poof -> into the painting!
            catPos = glm::vec3(-0.5f, groundLevel, 0.0f);  // drop her back at the left
            std::cout << "C.A.T.Scan activated! Jumping into the painting..." << std::endl;
        }
    }

  /* if (Keys[GLFW_KEY_S]) {
        catPos.y -= speed * dt;
      //swordPos = glm::vec3(catPos.x + catSize.x* 0.5f, catPos.y, 0.0f);
    }*/


    //SWORD MOVEMENT
    // the sword is thrown only if it is in state HELD and we press right click
    if(Keys[GLFW_MOUSE_BUTTON_RIGHT] && swordState == HELD ){
        swordState= THROWN;
        swordStartPos  = catPos;
        swordDir = facingDir;   // remember which way she was facing at throw time
        swordPos = catPos;
    }
     
    // dont go outside screen OX
    if (catPos.x > limitX) {
        catPos.x= limitX;
    }
    if (catPos.x < -limitX) {
        catPos.x = -limitX;
    }

    // Sdont go outside scrren OY
    if (catPos.y > limitY) {
        catPos.y = limitY;
    }
    if (catPos.y < -limitY) {
        catPos.y = -limitY;
    }
}

void Game::Update(float dt) {
    
       // === FIZICA GRAVITAȚIEI ȘI A SĂRITURII ===
    velocityY += gravity * dt;         // gravitația reduce viteza și o trage în jos
    catPos.y += velocityY * dt;         // deplasăm pisica pe axa Y
    // Dacă a atins sau a trecut sub nivelul pământului:
    if (catPos.y <= groundLevel) {
        catPos.y = groundLevel;         // o oprim fix pe pământ
        velocityY = 0.0f;              // oprim căderea
        catIsGrounded = true;             // a aterizat, acum poate sări din nou!
    }

    // land on the boss's head: if falling and above him while overlapping in X, stand on his head
    if (currentScene == SCENE_PAINTING && bossAlive && velocityY <= 0.0f) {
        float headY = bossPos.y + bossSize.y * 0.5f;              // top of the boss
        bool overX = fabs(catPos.x - bossPos.x) < (catSize.x + bossSize.x) * 0.5f;
        bool nearHead = catPos.y <= headY + 0.1f && catPos.y >= headY - 0.1f;
        if (overX && nearHead) {
            catPos.y = headY;           // stick her feet to his head
            velocityY = 0.0f;
            catIsGrounded = true;       // she can jump again from his head
        }
    }
    if(swordState == HELD) {
        swordPos = catPos;
        swordRotationAngle = 130.0f;
        glm::vec2 handOffset = glm::vec2(0.12f, -0.05f); // reglezi valorile ca manerul sa fie fix in labuta
        glm::vec2 drawSwordPos = glm::vec2(catPos.x + handOffset.x, catPos.y + handOffset.y);

} else if(swordState == THROWN) {
    swordPos.x += swordSpeed * swordDir * dt;   // fly in the thrown direction
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
    //coming back once it has traveled its max distance (either direction)
    if(fabs(swordPos.x - swordStartPos.x) >= swordMaxDistance) {
        swordState = RETURNING;
    }

} else if(swordState == RETURNING) {
    swordPos.x -= swordSpeed * swordDir * dt;   // fly back toward the cat (opposite of throw)
    swordRotationAngle += 1080.0f *dt;

    //we catch the sword only if we move the cat such that we catch it 
     if (CheckCollision(swordPos, swordSize, catPos, catSize)) {
        swordState = HELD;
    }
    //if it goes past the cat by 0.3 unit (in the return direction) we consider it dropped
       if (swordDir > 0.0f ? (swordPos.x < catPos.x - 0.3f) : (swordPos.x > catPos.x + 0.3f)) {
        swordState = GROUND;
    }
}else if(swordState == GROUND) {
    // the sword falls (gravity) until it rests on the floor where the cat walks
    swordPos.y -= 1.5f * dt;
    if (swordPos.y < groundLevel) {
        swordPos.y = groundLevel;   // stop it on the ground, same level as the cat
    }
    // it s not rotating anymore and we cath it by going to it
    swordRotationAngle = 0.0f;
    if (CheckCollision(swordPos, swordSize, catPos, catSize)) {
        swordState = HELD;
    }
 }

    // === PUZZLE PIECE (painting scene only) ===
    if (currentScene == SCENE_PAINTING && !puzzleCollected) {
        puzzleTimer += dt;   // clock used for the bob + spin animation

        // collect it when the cat walks into it
        if (CheckCollision(catPos, catSize, puzzlePos, puzzleSize)) {
            puzzleCollected = true;
            std::cout << "Fragment collected!" << std::endl;
        }
    }

    // === BOSS (painting scene only) ===
    if (currentScene == SCENE_PAINTING && bossAlive) {
        // patrol left <-> right between screen edges
        bossPos.x += bossSpeed * bossDir * dt;
        if (bossPos.x > 0.7f)  bossDir = -1.0f;   // hit right edge -> go left
        if (bossPos.x < -0.7f) bossDir = 1.0f;    // hit left edge -> go right

        // lance hitbox sits in front of him (in the direction he's facing/moving)
        lancePos = glm::vec3(bossPos.x + bossDir * (bossSize.x * 0.5f), bossPos.y, 0.0f);

        // sword hits the boss -> damage him
        if (swordState == THROWN && CheckCollision(swordPos, swordSize, bossPos, bossSize)) {
            bossHealth -= 34.0f;
            std::cout << "Boss lovit! Viata: " << bossHealth << std::endl;
            swordState = RETURNING;
            if (bossHealth <= 0.0f) {
                bossAlive = false;
                std::cout << "Boss invins! Ai castigat!" << std::endl;
            }
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



void Game::DrawBoss() {
    if (!bossAlive) return;
    // face the direction he is moving: art faces one way by default, flip with negative X size
    glm::vec2 drawSize = glm::vec2(bossSize.x * -bossDir, bossSize.y);
    Renderer->DrawSprite(bossTexture, glm::vec2(bossPos), drawSize, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}


void Game::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // --- Background depends on the scene ---
    if (currentScene == SCENE_MUSEUM) {
        // museum art (the C.A.T.Scan machine is painted into this image, on the left)
        Renderer->DrawSprite(museumTexture, glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 2.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    } else { // SCENE_PAINTING
        // for now reuse the loaded background image (swap your painting art in later)
        Renderer->DrawSprite(bgTexture, glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 2.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // --- The cat + sword (same in both scenes) ---
    // flip the cat horizontally by using a negative X size when facing left
    glm::vec2 catDrawSize = glm::vec2(catSize.x * facingDir, catSize.y);
    if (swordState == HELD) {
        // Sword is in her hand -> draw the cat-with-sword image, and NO separate sword
        Renderer->DrawSprite(catSwordTexture, glm::vec2(catPos), catDrawSize, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    } else {
        // Sword is thrown/flying/on the ground -> draw the plain cat AND the flying sword
        Renderer->DrawSprite(texture, glm::vec2(catPos), catDrawSize, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        Renderer->DrawSprite(swordTexture, glm::vec2(swordPos), swordSize, swordRotationAngle);
    }

    // --- Enemy only exists in the painting ---
    if (currentScene == SCENE_PAINTING) {
        // DrawEnemy();   // old primitive enemy (replaced by the boss)
        DrawBoss();

        // --- Puzzle piece: bob up/down (sin) + coin-flip on its axis (cos on X scale), until collected ---
        if (!puzzleCollected) {
            float bob = sin(puzzleTimer * 2.0f) * 0.08f;              // up/down wobble
            float flip = cos(puzzleTimer * 5.0f);                     // +1 -> 0 -> -1 -> ... : coin flip
            glm::vec2 drawPos = glm::vec2(puzzlePos.x, puzzlePos.y + bob);
            glm::vec2 flipSize = glm::vec2(puzzleSize.x * flip, puzzleSize.y);  // X scale drives the flip
            Renderer->DrawSprite(puzzleTexture, drawPos, flipSize, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
}

void Game::OnScroll(double yoffset) {
    // Zoom in / Zoom out cand invarti de rotita mouse-ului
    catSize += static_cast<float>(yoffset) * 0.05f;
    if (catSize.x < 0.1f) catSize = glm::vec2(0.1f, 0.1f);
    if (catSize.x > 1.5f) catSize = glm::vec2(1.5f, 1.5f);
}

Game::~Game() {
    delete Renderer;
    glDeleteTextures(1, &texture);
      glDeleteTextures(1, &bgTexture);
    glDeleteProgram(programID);
}