#pragma once

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "../renderer/shader.h"

using namespace geeL;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int a_window();
