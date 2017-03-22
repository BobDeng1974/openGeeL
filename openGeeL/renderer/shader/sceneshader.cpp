#include "sceneshader.h"

using namespace std;

namespace geeL {

	SceneShader::SceneShader() : Shader(), shader(""), cameraName("camera"), skyboxName("skybox") {}

	SceneShader::SceneShader(const FragmentShader& fragmentPath, bool animated, string cameraName, string skyboxName)
			: Shader(chooseVertexShader(fragmentPath, animated).c_str(), fragmentPath.path.c_str()), 
				shader(fragmentPath), cameraName(cameraName), skyboxName(skyboxName) {}


	void SceneShader::changeShader(const FragmentShader& shader) {
		init(vertexPath.c_str(), shader.path.c_str());
	}

	std::string SceneShader::chooseVertexShader(const FragmentShader& fragmentPath, bool animated) {
		if (animated && fragmentPath.deferred)
			vertexPath = "renderer/shaders/gbufferanim.vert";
		else if(!animated && fragmentPath.deferred)
			vertexPath = "renderer/shaders/gbuffer.vert";
		else if (animated && !fragmentPath.deferred)
			vertexPath = "renderer/shaders/lighting.vert"; //TODO: add skinned forward shader
		else if (!animated && !fragmentPath.deferred)
			vertexPath = "renderer/shaders/lighting.vert";

		return vertexPath;
	}
}