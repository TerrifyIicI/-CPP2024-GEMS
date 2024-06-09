#ifndef WINDOW_H
#define WINDOW_H


#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

static GLuint WIDTH = 600, HEIGHT = 800;


const int x_parts = 9, y_parts = 12;

// Function prototypes
void initGLFW();
void initGLEW();
void initOpenGL(GLFWwindow*& window);
void cleanup(GLuint& VAO, GLuint& VBO);

#endif#pragma once
