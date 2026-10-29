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

    // Pozitia de start: fix in centrul ecranului (0, 0)
    squarePos = glm::vec3(0.0f, 0.0f, 0.0f);
    // Marimea pisicii: 0.5 x 0.5 (exact ca inainte!)
    squareSize = glm::vec2(0.5f, 0.5f);
    // Viteza
    speed = 0.5f;

    Renderer = nullptr;
    memset(Keys, false, sizeof(Keys));
}

void Game::Init() {
    // 1. Incarcam shaderele
    programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");

    // 2. Initializam SpriteRenderer cu shaderul nostru!
    Renderer = new SpriteRenderer(programID);

    // 3. Incarcam imaginea pisicii (cat.png)
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
}

void Game::ProcessInput(float dt) {
    // Limita exacta pana la marginea ecranului:
    // (1.0 minus jumatate din dimensiunea patratului!)
    float limitX = 1.0f - (squareSize.x / 2.0f);
    float limitY = 1.0f - (squareSize.y / 2.0f);

    // Tastele A, D, W, S
    if (Keys[GLFW_KEY_A]) {
        squarePos.x -= speed * dt;
    }
    if (Keys[GLFW_KEY_D]) {
        squarePos.x += speed * dt;
    }
    if (Keys[GLFW_KEY_W]) {
        squarePos.y += speed * dt;
    }
    if (Keys[GLFW_KEY_S]) {
        squarePos.y -= speed * dt;
    }

    // Sa nu iasa din ecran pe X
    if (squarePos.x > limitX) {
        squarePos.x = limitX;
    }
    if (squarePos.x < -limitX) {
        squarePos.x = -limitX;
    }

    // Sa nu iasa din ecran pe Y
    if (squarePos.y > limitY) {
        squarePos.y = limitY;
    }
    if (squarePos.y < -limitY) {
        squarePos.y = -limitY;
    }
}

void Game::Update(float dt) {
    // Logica jocului (nu mai trebuie sa calculam matricea "trans" manual!)
}

void Game::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // DOAR O SINGURA LINIE PENTRU A DESENA!
    // Parametri: textura, pozitia (X, Y), marimea, rotatia (0 grade), culoarea (alb)
    Renderer->DrawSprite(texture, glm::vec2(squarePos), squareSize, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
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