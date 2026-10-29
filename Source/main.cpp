#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Game/game.h"

// Global pointer so callbacks can access the game
Game* gamePtr = nullptr;

//window size:
const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;


//CALLBACKS

/* ESC → close the window
Any other key → record it in gamePtr->Keys[] as true (pressed) or false (released)
This is why ProcessInput()  from game.cpp can just check Keys[GLFW_KEY_W] — this callback already filled it in.

cpp

*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            gamePtr->Keys[key] = true;
        else if (action == GLFW_RELEASE)
            gamePtr->Keys[key] = false;
    }
}

void window_callback(GLFWwindow* window, int new_width, int new_height) {
    glViewport(0, 0, new_width, new_height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    gamePtr->OnScroll(yoffset);
}

int main(void) {

	//GLFW + GLEW initialization
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2D Game", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Set callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, window_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Create game object, points gamePtr to it so the callback cand find it , calls Init() fir shaders,  gemorty and textures
    Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
    gamePtr = &game;
    game.Init();

    // Game loop with delta time
	//check if the user did anything
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float dt = currentFrame - lastFrame;
		if (dt > 0.05f) dt = 0.05f;
        lastFrame = currentFrame;

        glfwPollEvents();

		// 3 steps: read  input -> update logic - > draw
        game.ProcessInput(dt);
        game.Update(dt);
        game.Render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}