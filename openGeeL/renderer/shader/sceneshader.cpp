#include "sceneshader.h"
#include <iostream>

using namespace std;

namespace geeL {

	SceneShader::SceneShader() : Shader(), shader(""), space(ShaderTransformSpace::View), cameraName("camera"),
		skyboxName("skybox"), view(nullptr), model(nullptr) {}

	SceneShader::SceneShader(const std::string& vertexPath, const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, bool animated, string cameraName, string skyboxName)
			: Shader(vertexPath.c_str(), fragmentPath.path.c_str()), shader(fragmentPath), space(space), 
				cameraName(cameraName), skyboxName(skyboxName), view(nullptr), model(nullptr) {

		viewLocation = getLocation("viewMatrix");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
	}

	SceneShader::SceneShader(const std::string& vertexPath, const std::string& geometryPath, const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, bool animated, string cameraName, string skyboxName)
			: Shader(vertexPath.c_str(), geometryPath.c_str(), fragmentPath.path.c_str()), shader(fragmentPath), space(space), cameraName(cameraName), 
				skyboxName(skyboxName), view(nullptr), model(nullptr) {

		viewLocation = getLocation("viewMatrix");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
	}


	void SceneShader::changeShader(const FragmentShader& shader) {
		init(vertexPath.c_str(), shader.path.c_str());
	}

	void SceneShader::setViewMatrix(const glm::mat4& view) {
		this->view = &view;
	}

	void SceneShader::setModelMatrix(const glm::mat4& model) {
		this->model = &model;
	}

	void SceneShader::bindViewMatrix() const {
		if (view != nullptr)
			setMat4(viewLocation, *view);
	}

	void SceneShader::bindModel() const {
		if (model != nullptr)
			setMat4(modelLocation, *model);
	}

	void SceneShader::bindMatrices() const {
		if (view != nullptr && model != nullptr) {
			setMat4(modelViewLocation, (*view) * (*model));
		}
	}

}