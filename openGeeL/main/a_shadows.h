#pragma once

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <SOIL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../renderer/shader/shader.h"
#include "../renderer/renderer/splitrenderer.h"
#include "../renderer/renderer/simplerenderer.h"

#include "../renderer/renderobject.h"
#include "../renderer/inputmanager.h"
#include "../renderer/window.h"
#include "../renderer/texturing/texture.h"
#include "../renderer/texturing/simpletexture.h"
#include "../renderer/texturing/layeredtexture.h"

using namespace geeL;

void a_shadows();

