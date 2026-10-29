#include "game.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../stb_image.h"
#include "../Shaders/Utils/shaderLoader.hpp"
#include <iostream>
#include <cstring>

Game::Game(unsigned int width, unsigned int height) {
    // Set the values that were passed in
    Width = width;
    Height = height;
    State = GAME_ACTIVE;

    // Set initial game state - look at your main.cpp for these values
    squarePos = glm::vec3(0.0f, 0.0f, 0.0f);
    zoomScale = 1.0f;
    speed = 0.5f;
    trans = glm::mat4(1.0f);
    limit = 1.0f - (0.5f * zoomScale);

    // Zero out the keys array
    memset(Keys, false, sizeof(Keys));
}

void Game::Init() {

    programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");

	GLfloat vertices[] = {
		// X,      Y,     Z,      U,    V
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};

	GLuint indices[] = {	// Note that we start from 0
		0, 3, 1,			// first Triangle
		1, 3, 2,			// second Triangle
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	// Bind VAO
	glBindVertexArray(vao);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Describe the data in the buffer
	// This will be accesible in the shaders
	//ATTRIBUTE 0: POSITION X, Y, Z
	glVertexAttribPointer
	(
		0,                    // attribute 0, must match the layout location in the shader
		3,                    // size of each attribute (3 floats)
		GL_FLOAT,             // type of data
		GL_FALSE,             // should be normalized?
		5 * sizeof(GL_FLOAT), // stride for this attribute
		(void*)0			  // initial offset for array buffer
	);
	glEnableVertexAttribArray(0);
	
	//ATTRIBUTE 1: TEXTURE COORD : U, V
	glVertexAttribPointer
	(
		1,                   
		2,                   
		GL_FLOAT,           
		GL_FALSE,             
		5 * sizeof(GL_FLOAT), 
		(void*)(3 * sizeof(GL_FLOAT))			 
	);
	glEnableVertexAttribArray(1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;

	// ATENȚIE: Folosește dublu backslash (\\) pentru calea absolută!
	unsigned char* data = stbi_load("cat.png", &width, &height, &nrChannels, 4);

	if (data) {
		// Forțăm GL_RGBA deoarece am pus 4 la parametrul anterior
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Imagine incarcata cu succes! Dimensiuni: " << width << "x" << height << std::endl;
	}
	else {
		std::cout << "EROARE CRITICA: Imaginea nu a fost gasita la calea specificata!" << std::endl;
	}
	stbi_image_free(data);
}

void Game::ProcessInput(float dt) {
    limit = 1.0f - (0.5f * zoomScale);
    //tastele A,W,S,D 

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

    //sa nu ies in afara window  ului
    if (squarePos.x > limit) {
        squarePos.x = limit;
    }
    if (squarePos.x < -limit) {
        squarePos.x = -limit;
    }

    // Constrain the Y axis
    if (squarePos.y > limit) {
        squarePos.y = limit;
    }
    if (squarePos.y < -limit) {
        squarePos.y = -limit;
    }
}

void Game::Update(float dt) {
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, squarePos);
    trans = glm::scale(trans, glm::vec3(zoomScale, zoomScale, 0.0f));
}

void Game::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programID);

    glBindVertexArray(vao);

    unsigned int transformLoc = glGetUniformLocation(programID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glUniform1i(glGetUniformLocation(programID, "ourTexture"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Game::OnScroll(double yoffset) {
    zoomScale += yoffset * 0.1f;
    if (zoomScale <= 0.1f)
        zoomScale = 0.1f;
}

Game::~Game() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}
