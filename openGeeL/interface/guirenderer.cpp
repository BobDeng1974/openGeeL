#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#include "guiwrapper.h"


#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include "renderer/rendercontext.h"
#include "renderer/deferredrenderer.h"
#include "window.h"
#include "elements/systeminformation.h"
#include "guielement.h"
#include "guirenderer.h"


namespace geeL {

	GUIRenderer::GUIRenderer(RenderWindow& window, 
		RenderContext& context, 
		DeferredRenderer& renderer) 
			: renderContext(&context)
			, renderer(renderer)
			, window(window) {
		
		init(window);
	}

	GUIRenderer::~GUIRenderer() {
		for (auto element(elements.begin()); element != elements.end(); element++)
			if(element->first) delete element->second;
	}


	void GUIRenderer::init(const RenderWindow& window) {

		guiContext = nk_glfw3_init(window.glWindow, NK_GLFW3_INSTALL_CALLBACKS);
		/* Load Fonts: if none of these are loaded a default font will be used  */
		/* Load Cursor: if you uncomment cursor loading please hide the cursor */
		struct nk_font_atlas *atlas;
		nk_glfw3_font_stash_begin(&atlas);
		nk_glfw3_font_stash_end();
	}


	void GUIRenderer::draw() {

		nk_glfw3_new_frame();

		for (auto element(elements.begin()); element != elements.end(); element++) {
			GUIElement& e = *element->second;
			e.draw(guiContext);
		}

		nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		renderContext->reset();
	}

	void GUIRenderer::addElement(GUIElement& element) {
		elements.push_back(std::pair<bool, GUIElement*>(false, &element));
	}
	void GUIRenderer::addSystemInformation(float x, float y, float width, float height) {
		elements.push_back(std::pair<bool, GUIElement*>(true, 
			new SystemInformation(window, renderer, x, y, width, height)));

	}

}