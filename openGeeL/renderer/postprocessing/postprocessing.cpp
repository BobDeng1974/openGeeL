#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "postprocessing.h"

namespace geeL {

	PostProcessingEffect::PostProcessingEffect(string shaderPath) 
		: shader(Shader("renderer/shaders/screen.vert", shaderPath.c_str())) {}

	void PostProcessingEffect::bindToScreen(GLuint screenVAO, GLuint colorBuffer) {

		//shader.use();
		glBindVertexArray(screenVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}