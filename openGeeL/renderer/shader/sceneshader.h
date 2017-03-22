#ifndef SCENESHADER_H
#define SCENESHADER_H

#include <string>
#include "defshading.h"
#include "shader.h"

namespace geeL {


	struct FragmentShader {
		const std::string path;
		const bool deferred;
		const bool useLight;
		const bool useCamera;
		const bool useSkybox;


		FragmentShader(std::string path, bool deferred = true, bool useLight = true, bool useCamera = true, bool useSkybox = true) 
			: path(path), deferred(deferred), useLight(useLight), useCamera(useCamera), useSkybox(useSkybox) {}
	};

	//Special shader container that is intented for the use in scene drawing.
	//It therefore holds additional scene information.
	class SceneShader : public Shader {

	public:
		std::string cameraName, skyboxName;

		SceneShader();

		SceneShader(const FragmentShader& fragmentPath, bool animated = false,
			std::string cameraName = "camera", std::string skyboxName = "skybox");

		void changeShader(const FragmentShader& shader);

		bool SceneShader::getDeferred() const;
		bool SceneShader::getUseLight() const;
		bool SceneShader::getUseCamera() const;
		bool SceneShader::getUseSkybox() const;

	private:
		FragmentShader shader;
		std::string vertexPath;

		std::string chooseVertexShader(const FragmentShader& fragmentPath, bool animated);

	};


	inline bool SceneShader::getDeferred() const {
		return shader.deferred;
	}

	inline bool SceneShader::getUseLight() const {
		return shader.useLight;
	}

	inline bool SceneShader::getUseCamera() const {
		return shader.useCamera;
	}

	inline bool SceneShader::getUseSkybox() const {
		return shader.useSkybox;
	}
}

#endif 

