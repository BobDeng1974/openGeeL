#ifndef SCENESHADER_H
#define SCENESHADER_H

#include <string>
#include "defshading.h"
#include "rendershader.h"

namespace geeL {


	struct FragmentShader {
		const std::string path;
		const bool useLight;
		const bool useCamera;
		const bool useSkybox;


		FragmentShader(std::string path, bool useLight = true, bool useCamera = true, bool useSkybox = true)
			: path(path), useLight(useLight), useCamera(useCamera), useSkybox(useSkybox) {}
	};



	//Special shader container that is intented for the use in scene drawing.
	//It therefore holds additional scene information.
	class SceneShader : public RenderShader {

	public:
		SceneShader();

		SceneShader(const std::string& vertexPath, const FragmentShader& fragmentPath, ShaderTransformSpace space, 
			ShadingMethod shadingMethod, ShaderProvider* const provider = nullptr);

		SceneShader(const std::string& vertexPath, const std::string& geometryPath, const FragmentShader& fragmentPath, 
			ShaderTransformSpace space, ShadingMethod shadingMethod, ShaderProvider* const provider = nullptr);


		bool SceneShader::getUseLight() const;
		bool SceneShader::getUseCamera() const;
		bool SceneShader::getUseSkybox() const;

		void setViewMatrix(const glm::mat4& view);

		//Bind model and view matrices into shader (if view has been set beforehand)
		void bindMatrices(const glm::mat4& model) const;
		void bindViewMatrix() const;

		ShaderTransformSpace getSpace() const;
		ShadingMethod getMethod() const;

	private:
		FragmentShader shader;
		ShaderTransformSpace space;
		ShadingMethod shadingMethod;
		std::string vertexPath;

		const glm::mat4* view;
		glm::mat4 model;

		ShaderLocation viewLocation;
		ShaderLocation modelLocation;
		ShaderLocation modelViewLocation;
		ShaderLocation transInvModelViewLocation;

	};

	inline bool SceneShader::getUseLight() const {
		return shader.useLight;
	}

	inline bool SceneShader::getUseCamera() const {
		return shader.useCamera;
	}

	inline bool SceneShader::getUseSkybox() const {
		return shader.useSkybox;
	}

	inline ShaderTransformSpace SceneShader::getSpace() const {
		return space;
	}

	inline ShadingMethod SceneShader::getMethod() const {
		return shadingMethod;
	}

}

#endif 