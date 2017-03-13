#include "shader.h"

using namespace std;

namespace geeL {

	SceneShader::SceneShader() : Shader(), deferred(false), useLight(false), 
		useCamera(false), useSkybox(false), cameraName("camera"), skyboxName("skybox") {}

	SceneShader::SceneShader(const char* vertexPath, const char* fragmentPath, bool deferred, bool useLight,
		bool useCamera, bool useSkybox, string cameraName, string skyboxName)
			: Shader(vertexPath, fragmentPath),  deferred(deferred), useLight(useLight), useCamera(useCamera), 
				useSkybox(useSkybox), cameraName(cameraName), skyboxName(skyboxName) {}

	SceneShader::SceneShader(const char* vertexPath, const char* geometryPath, const char* fragmentPath, bool deferred, bool useLight,
		bool useCamera, bool useSkybox, string cameraName, string skyboxName)
			: Shader(vertexPath, geometryPath, fragmentPath),  deferred(deferred), useLight(useLight), 
				useCamera(useCamera), useSkybox(useSkybox), cameraName(cameraName), skyboxName(skyboxName) {}

}