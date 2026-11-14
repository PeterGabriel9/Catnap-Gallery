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
    
    cameraX = 0.0f;

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
  
  //viata pisiii
    catMaxHealth = 6.0f;    // Viata maxima (6)
    catHealth = 6.0f;       // Viata curenta
    catHurtTimer= 0.0f;    // Timp de imunitate dupa ce este lovita
    

    swordPos = glm::vec3(catPos.x + catSize.x* 0.5f, catPos.y, 0.0f);
    swordSize = glm::vec2(0.2f, 0.4f);
    swordSpeed = 0.9f;
    swordMaxDistance = 1.3f;
    swordRotationAngle = 45.0f;
    swordState = HELD;
    swordDir = 1.0f;   // default throw direction

    //ENEMY info
    enemyPos = glm::vec3(0.7f, 0.0f, 0.0f);       
    enemySize = glm::vec2(0.25f, 0.35f);
    enemyMaxHealth = 6.0f;
    enemyHealth = 6.0f;
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
    bossHurtTimer = 0.0f;
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


        unsigned char* heartData = stbi_load("assets/heart.png", &w, &h, &nrChannels, 4);
    glGenTextures(1, &heartTexture);
    glBindTexture(GL_TEXTURE_2D, heartTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (heartData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, heartData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(heartData);
 

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
       // Detectam o singura apasare de tasta E (evita declansarea dubla instantanee)
    static bool eKeyWasPressed = false;
    bool eJustPressed = Keys[GLFW_KEY_E] && !eKeyWasPressed;
    eKeyWasPressed = Keys[GLFW_KEY_E];

    if (eJustPressed) {
        // 1. Daca esti in Muzeu langa masina -> intri in Tablou:
        if (currentScene == SCENE_MUSEUM) {
            if (CheckCollision(catPos, catSize, machinePos, machineSize)) {
                currentScene = SCENE_PAINTING;
                catPos = glm::vec3(-0.5f, groundLevel, 0.0f);
                std::cout << "C.A.T.Scan activat! Ai sarit in tablou..." << std::endl;
            }
        } 
        // 2. Daca esti in Tablou -> te intorci in Muzeu direct in fata tabloului:
        else if (currentScene == SCENE_PAINTING) {
            currentScene = SCENE_MUSEUM;
            catPos = glm::vec3(0.15f, groundLevel, 0.0f);
            cameraX = 0.0f;
            std::cout << "Te-ai intors in Muzeu cu piesa!" << std::endl;
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
    if (currentScene == SCENE_MUSEUM) {
        if (catPos.x > limitX)  catPos.x = limitX;
        if (catPos.x < -limitX) catPos.x = -limitX;
    } else {
        // In TABLOU: marginea din stanga urmareste camera ca sa nu dispari din ecran cand fugi!
          float minWorldX = -1.5f;
        if (catPos.x < minWorldX) {
            catPos.x = minWorldX;
        }
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
      
     // === CAMERA SMOOTH FOLLOW (inainte si inapoi) ===
    if (currentScene == SCENE_PAINTING) {
        float targetCamX = catPos.x + 0.2f;
        if (targetCamX < -0.8f) targetCamX = -0.8f;// permite pisicii si camerei sa se retraga pe -X
        // Camera pluteste lin dupa pisica (Lerp):
        cameraX += (targetCamX - cameraX) * 5.0f * dt;
    } else {
        cameraX = 0.0f; // In muzeu camera ramane pe loc
    }

      if (catHurtTimer > 0.0f) {
        catHurtTimer -= dt;
    }
      if (bossHurtTimer > 0.0f) {
        bossHurtTimer -= dt;
    }

    // === 1. FIZICA GRAVITAȚIEI ȘI A SĂRITURII ===
    velocityY += gravity * dt;         // gravitația trage în jos
    catPos.y += velocityY * dt;        // deplasare pe axa Y
    if (catPos.y <= groundLevel) {
        catPos.y = groundLevel;        // oprire la nivelul solului
        velocityY = 0.0f;
        catIsGrounded = true;          // a atins solul
    }

    // === 2. ATERIZARE / STAT PE CAPUL SAU SPATELE BOSS-ULUI ===
    bool standingOnBoss = false;
    float bossHeadLevel = bossPos.y + 0.50f; // inaltimea unde stau labutele pe cap/sa
    bool overBossX = fabs(catPos.x - bossPos.x) < 0.40f;

    if (currentScene == SCENE_PAINTING && bossAlive) {
        // Daca pisica e deasupra boss-ului si pica spre el (sau este deja pe el):
        if (overBossX && catPos.y >= bossPos.y + 0.25f && catPos.y <= bossHeadLevel + 0.18f && velocityY <= 0.0f) {
            catPos.y = bossHeadLevel;
            velocityY = 0.0f;
            catIsGrounded = true;                 // poate sari din nou direct de pe capul lui!
            standingOnBoss = true;
            catPos.x += bossSpeed * bossDir * dt; // pisica calatoreste sincronizata cu boss-ul!
        }
    }

    // === 3. COLIZIUNE CU CORPUL BOSS-ULUI (DAMAGE PISICA) ===
    glm::vec2 catHitbox = glm::vec2(0.25f, 0.35f);
    glm::vec2 bossHitbox = glm::vec2(0.45f, 0.55f);

    // Ia damage DOAR daca vine din lateral/jos (NU si cand sta pe capul lui) si a expirat imunitatea:
    if (currentScene == SCENE_PAINTING && bossAlive && !standingOnBoss && catHurtTimer <= 0.0f) {
        if (CheckCollision(catPos, catHitbox, bossPos, bossHitbox)) {
            catHealth -= 1.0f;            // scadem 1 segment de viata
            catHurtTimer = 1.0f;          // 1 secunda de imunitate (clipeste rosu)
            catPos.x -= facingDir * 0.2f; // knockback inapoi
            std::cout << "Pisica a fost lovita de Boss! Viata ramasa: " << catHealth << std::endl;
            if (catHealth <= 0.0f) {
                std::cout << "Pisica a fost invinsa! Game Over!" << std::endl;
            }
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
     if (CheckCollision(swordPos, swordSize, catPos, glm::vec2(0.18f, 0.30f))) {
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
    if (CheckCollision(swordPos, swordSize, catPos, glm::vec2(0.18f, 0.30f))) {
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
        if (bossPos.x > 0.2f)  bossDir = -1.0f;   // hit right edge -> go left
       if (bossPos.x < -0.2f) bossDir = 1.0f;    // intoarce la -0.2 ca sa nu mai intre peste tine

        // lance hitbox sits in front of him (in the direction he's facing/moving)
        lancePos = glm::vec3(bossPos.x + bossDir * (bossSize.x * 0.5f), bossPos.y, 0.0f);

        // sword hits the boss -> damage him
           glm::vec2 bossSwordHitbox = glm::vec2(0.40f, 0.55f);
        if (swordState == THROWN && CheckCollision(swordPos, swordSize, bossPos, bossSwordHitbox)) {
                        bossHealth -= 34.0f;
            bossHurtTimer = 0.25f; // calul clipeste rosu intens la impact!
            std::cout << "Boss lovit! Viata: " << bossHealth << std::endl;
            swordState = RETURNING;
            if (bossHealth <= 0.0f) {
                bossAlive = false;
                std::cout << "Boss invins! Ai castigat!" << std::endl;
            }
        }
    }
}
 /*
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

*/

void Game::DrawBoss() {
    if (!bossAlive) return;
    // face the direction he is moving: art faces one way by default, flip with negative X size
    glm::vec2 drawSize = glm::vec2(bossSize.x * -bossDir, bossSize.y);
       glm::vec3 bossColor = (bossHurtTimer > 0.0f) ? glm::vec3(1.0f, 0.3f, 0.3f) : glm::vec3(1.0f, 1.0f, 1.0f);
    Renderer->DrawSprite(bossTexture, glm::vec2(bossPos.x - cameraX, bossPos.y), drawSize, 0.0f, bossColor);

}

// Piesa de puzzle 2D construita din 6 primitive geometrice complet sudate
// Obiect complex compus din 15 primitive geometrice (Tablou de muzeu in rama aurita)
void Game::DrawFramedPainting(glm::vec2 pos, glm::vec2 size) {
    // Dimensiunea panzei din interior
    glm::vec2 canvasSize = glm::vec2(size.x * 0.82f, size.y * 0.82f);
      // ==========================================
    // 1. PANZA GRI (ARTEFACTUL RUPT / LIPSĂ)
    // ==========================================
    // Fundalul gri inchis al panzei deteriorate
    Renderer->DrawSprite(whiteTexture, pos, canvasSize, 0.0f, glm::vec3(0.20f, 0.20f, 0.24f));
    // Slotul piesei lipsa din mijlocul panzei
    glm::vec2 slotSize = glm::vec2(canvasSize.x * 0.42f, canvasSize.y * 0.42f);
    
    if (!puzzleCollected) {
        // Cand piesa lipseste: decupaj intunecat cu marcaje de contur
        Renderer->DrawSprite(whiteTexture, pos, slotSize, 0.0f, glm::vec3(0.12f, 0.12f, 0.15f));
        float markW = slotSize.x * 0.20f;
        float markH = 0.008f;
        glm::vec3 markCol = glm::vec3(0.55f, 0.50f, 0.35f);
        Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y + slotSize.y * 0.5f), glm::vec2(markW, markH), 0.0f, markCol);
        Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y - slotSize.y * 0.5f), glm::vec2(markW, markH), 0.0f, markCol);
        Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x - slotSize.x * 0.5f, pos.y), glm::vec2(markH, markW), 0.0f, markCol);
        Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + slotSize.x * 0.5f, pos.y), glm::vec2(markH, markW), 0.0f, markCol);
      } else {
        // Cand piesa a fost recuperata: piesa de puzzle se potriveste perfect in tablou!
        DrawPuzzlePiece(pos, 0.12f);
    }
    // 2. RAMA AURITA BAROCA (4 LATURI + 4 COLTARE)
    // ==========================================
    float borderW = size.x * 0.09f;
    float borderH = size.y * 0.09f;

    // Latura de sus
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y + size.y * 0.455f), glm::vec2(size.x, borderH), 0.0f, glm::vec3(0.85f, 0.68f, 0.20f));
    // Latura de jos
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y - size.y * 0.455f), glm::vec2(size.x, borderH), 0.0f, glm::vec3(0.65f, 0.48f, 0.12f));
    // Latura stanga
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x - size.x * 0.455f, pos.y), glm::vec2(borderW, size.y), 0.0f, glm::vec3(0.75f, 0.55f, 0.15f));
    // Latura dreapta
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + size.x * 0.455f, pos.y), glm::vec2(borderW, size.y), 0.0f, glm::vec3(0.70f, 0.52f, 0.14f));

    // Cele 4 coltare decorative aurii
    glm::vec2 cornerSize = glm::vec2(borderW * 1.3f, borderH * 1.3f);
    glm::vec3 cornerColor = glm::vec3(0.95f, 0.82f, 0.30f);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x - size.x * 0.45f, pos.y + size.y * 0.45f), cornerSize, 0.0f, cornerColor);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + size.x * 0.45f, pos.y + size.y * 0.45f), cornerSize, 0.0f, cornerColor);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x - size.x * 0.45f, pos.y - size.y * 0.45f), cornerSize, 0.0f, cornerColor);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + size.x * 0.45f, pos.y - size.y * 0.45f), cornerSize, 0.0f, cornerColor);

    // ==========================================
    // 3. PLACUTA MUZEALA DE ALAMA
    // ==========================================
    glm::vec2 plaquePos = glm::vec2(pos.x, pos.y - size.y * 0.58f);
    glm::vec2 plaqueSize = glm::vec2(size.x * 0.42f, size.y * 0.09f);

    // Placa de alama
    Renderer->DrawSprite(whiteTexture, plaquePos, plaqueSize, 0.0f, glm::vec3(0.82f, 0.70f, 0.32f));
    // Textul/gravura intunecata din mijloc
    Renderer->DrawSprite(whiteTexture, plaquePos, glm::vec2(plaqueSize.x * 0.65f, plaqueSize.y * 0.22f), 0.0f, glm::vec3(0.25f, 0.18f, 0.05f));
    // Surub stanga
    Renderer->DrawSprite(whiteTexture, glm::vec2(plaquePos.x - plaqueSize.x * 0.42f, plaquePos.y), glm::vec2(0.012f, 0.012f), 0.0f, glm::vec3(0.2f, 0.15f, 0.05f));
    // Surub dreapta
    Renderer->DrawSprite(whiteTexture, glm::vec2(plaquePos.x + plaqueSize.x * 0.42f, plaquePos.y), glm::vec2(0.012f, 0.012f), 0.0f, glm::vec3(0.2f, 0.15f, 0.05f));
}
// Piesa de puzzle autentica construita din 14 primitive geometrice
void Game::DrawPuzzlePiece(glm::vec2 pos, float size) {
    // Corectie pentru ecranul 4:3 (1024x768) ca piesa sa fie un patrat perfect, nu turtita
    float aspect = 1024.0f / 768.0f;
    float sx = size / aspect; // latimea corectata
    float sy = size;          // inaltimea

    // Paleta de culori aurii cu lumini si umbre
    glm::vec3 goldBase  = glm::vec3(0.92f, 0.72f, 0.18f); // auriu cald
    glm::vec3 goldLight = glm::vec3(1.0f, 0.88f, 0.38f);  // muchie luminata
    glm::vec3 goldDark  = glm::vec3(0.68f, 0.48f, 0.08f);  // muchie umbrita
    glm::vec3 holeDark  = glm::vec3(0.10f, 0.12f, 0.16f);  // golul decupajului

    // 1. Corpul central al piesei
    Renderer->DrawSprite(whiteTexture, pos, glm::vec2(sx, sy), 0.0f, goldBase);

    // Muchiile luminate de sus si stanga (efect de volum 2D)
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y + sy * 0.46f), glm::vec2(sx, sy * 0.08f), 0.0f, goldLight);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x - sx * 0.46f, pos.y), glm::vec2(sx * 0.08f, sy), 0.0f, goldLight);
    // Muchiile umbrite de jos si dreapta
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y - sy * 0.46f), glm::vec2(sx, sy * 0.08f), 0.0f, goldDark);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + sx * 0.46f, pos.y), glm::vec2(sx * 0.08f, sy), 0.0f, goldDark);

    // 2. Urechiusa de sus (gat ingust + cap lat de puzzle)
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y + sy * 0.58f), glm::vec2(sx * 0.28f, sy * 0.16f), 0.0f, goldBase);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y + sy * 0.80f), glm::vec2(sx * 0.46f, sy * 0.28f), 0.0f, goldBase);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y + sy * 0.90f), glm::vec2(sx * 0.46f, sy * 0.08f), 0.0f, goldLight);

    // 3. Urechiusa din dreapta (gat ingust + cap lat de puzzle)
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + sx * 0.58f, pos.y), glm::vec2(sx * 0.16f, sy * 0.28f), 0.0f, goldBase);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + sx * 0.80f, pos.y), glm::vec2(sx * 0.28f, sy * 0.46f), 0.0f, goldBase);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x + sx * 0.90f, pos.y), glm::vec2(sx * 0.08f, sy * 0.46f), 0.0f, goldDark);

    // 4. Scobitura din stanga (soclu de decupaj cu umbra)
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x - sx * 0.32f, pos.y), glm::vec2(sx * 0.38f, sy * 0.38f), 0.0f, holeDark);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x - sx * 0.14f, pos.y), glm::vec2(sx * 0.04f, sy * 0.38f), 0.0f, goldDark);

    // 5. Scobitura de jos (soclu de decupaj cu umbra)
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y - sy * 0.32f), glm::vec2(sx * 0.38f, sy * 0.38f), 0.0f, holeDark);
    Renderer->DrawSprite(whiteTexture, glm::vec2(pos.x, pos.y - sy * 0.14f), glm::vec2(sx * 0.38f, sy * 0.04f), 0.0f, goldDark);

    // 6. Diamantul / Miezul magic stralucitor din centru
    Renderer->DrawSprite(whiteTexture, pos, glm::vec2(sx * 0.30f, sy * 0.30f), 0.785f, glm::vec3(1.0f, 1.0f, 0.85f));
    Renderer->DrawSprite(whiteTexture, pos, glm::vec2(sx * 0.16f, sy * 0.16f), 0.785f, glm::vec3(1.0f, 1.0f, 1.0f));
}
  
void Game::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // --- Background depends on the scene ---
    if (currentScene == SCENE_MUSEUM) {
        // museum art (the C.A.T.Scan machine is painted into this image, on the left)
        Renderer->DrawSprite(museumTexture, glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 2.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        DrawFramedPainting(glm::vec2(0.25f, 0.15f), glm::vec2(0.48f, 0.38f));
    } else { // SCENE_PAINTING
    float bgOffset = fmod(cameraX * 0.3f, 2.0f);
    if (bgOffset < 0.0f) bgOffset += 2.0f; // asigura ca fundalul se continua perfect si pe -X
    Renderer->DrawSprite(bgTexture, glm::vec2(-bgOffset, 0.0f), glm::vec2(2.0f, 2.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    Renderer->DrawSprite(bgTexture, glm::vec2(2.0f - bgOffset, 0.0f), glm::vec2(2.0f, 2.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

    // --- The cat + sword (same in both scenes) ---
    // flip the cat horizontally by using a negative X size when facing left
    glm::vec2 catDrawSize = glm::vec2(catSize.x * facingDir, catSize.y);
    glm::vec3 catColor = (catHurtTimer > 0.0f) ? glm::vec3(1.0f, 0.4f, 0.4f) : glm::vec3(1.0f, 1.0f, 1.0f);
   
    float screenCatX = catPos.x - cameraX;
      if (swordState == HELD) {
        Renderer->DrawSprite(catSwordTexture, glm::vec2(screenCatX, catPos.y), catDrawSize, 0.0f, catColor);
    } else {
        Renderer->DrawSprite(texture, glm::vec2(screenCatX, catPos.y), catDrawSize, 0.0f, catColor);
        Renderer->DrawSprite(swordTexture, glm::vec2(swordPos.x - cameraX, swordPos.y), swordSize, swordRotationAngle);
    }

       // --- Enemy only exists in the painting ---
    if (currentScene == SCENE_PAINTING) {
        // DrawEnemy();   // old primitive enemy (replaced by the boss)
        DrawBoss();

        // --- Piesa de puzzle autentica din primitive (plutire lina pe verticala) ---
        if (!puzzleCollected) {
            float bob = sin(puzzleTimer * 2.5f) * 0.06f; // plutire lina sus-jos
            glm::vec2 drawPos = glm::vec2(puzzlePos.x - cameraX, puzzlePos.y + bob);
            DrawPuzzlePiece(drawPos, 0.16f);
        }
    }
            // ==========================================
    // HUD: BARA DE VIAȚĂ A PISICII (Stânga-Sus)
    // ==========================================
    float hudX = -0.75f;
    float hudY = 0.85f;


        // 1. Iconița inimioară roșie (desenată în stânga barei)
    Renderer->DrawSprite(heartTexture, glm::vec2(hudX - 0.055f, hudY), glm::vec2(0.045f, 0.045f), 0.0f);
    // 2. Rama neagră din spate
    Renderer->DrawSprite(whiteTexture, glm::vec2(hudX + 0.125f, hudY), glm::vec2(0.32f, 0.045f), 0.0f, glm::vec3(0.1f, 0.1f, 0.1f));
    // 3. Cele 6 segmente de viață (verzi când sunt active, gri când sunt pierdute)
    glm::vec2 catSegSize = glm::vec2(0.045f, 0.03f);
    float catSegSpacing = 0.05f;
    for (int i = 0; i < 6; i++) {
        bool isActive = (catHealth > i);
        glm::vec3 segColor = isActive ? glm::vec3(0.1f, 0.9f, 0.3f) : glm::vec3(0.25f, 0.25f, 0.25f);
        Renderer->DrawSprite(whiteTexture, glm::vec2(hudX + i * catSegSpacing, hudY), catSegSize, 0.0f, segColor);
    }
     // ==========================================
    // HUD: BARA DE VIAȚĂ A BOSS-ULUI (Dreapta-Sus)
    // ==========================================
    if (currentScene == SCENE_PAINTING && bossAlive) {
        float screenBossX = bossPos.x - cameraX;
        float bossHudX = screenBossX - 0.05f;
        float bossHudY = bossPos.y - 0.35f;
        // 1. Rama neagra din spate
        Renderer->DrawSprite(whiteTexture, glm::vec2(screenBossX, bossHudY), glm::vec2(0.17f, 0.045f), 0.0f, glm::vec3());
        // 2. Cele 6 segmente rosii ale Boss-ului (se sting pe masura ce este lovit)
        glm::vec2 bossSegSize = glm::vec2(0.045f, 0.03f);
        float bossSegSpacing = 0.05f;
        int activeBossSegments = (int)ceil((bossHealth / bossMaxHealth) * 3.0f);
        for (int i = 0; i < 3; i++) {
            bool isActive = (i < activeBossSegments);
            // Rosu intens cand are viata, gri inchis cand pierde viata:
            glm::vec3 segColor = isActive ? glm::vec3(0.9f, 0.15f, 0.15f) : glm::vec3(0.25f, 0.25f, 0.25f);
            Renderer->DrawSprite(whiteTexture, glm::vec2(bossHudX + i * bossSegSpacing, bossHudY), bossSegSize, 0.0f, segColor);
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
    glDeleteTextures(1, &heartTexture);
    glGenTextures(1, &bossTexture);  
    glGenTextures(1, &catSwordTexture);
    glGenTextures(1, &museumTexture);
           

    glDeleteProgram(programID);
}