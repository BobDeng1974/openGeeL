#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "sceneshader.h"

using namespace std;

namespace geeL {

	SceneShader::SceneShader() : RenderShader(), shader(""), 
		space(ShaderTransformSpace::View), view(nullptr) {}

	SceneShader::SceneShader(const std::string& vertexPath, const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, ShadingMethod shadingMethod, ShaderProvider* const provider)
			: RenderShader(vertexPath.c_str(), fragmentPath.path.c_str(), provider), shader(fragmentPath), space(space), 
				shadingMethod(shadingMethod), view(nullptr) {

		viewLocation = getLocation("view");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
		transInvModelViewLocation = getLocation("transInvModelView");
	}

	SceneShader::SceneShader(const std::string& vertexPath, const std::string& geometryPath, const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, ShadingMethod shadingMethod, ShaderProvider* const provider)
			: RenderShader(vertexPath.c_str(), geometryPath.c_str(), fragmentPath.path.c_str(), provider), shader(fragmentPath), 
				space(space), shadingMethod(shadingMethod), view(nullptr) {

		viewLocation = getLocation("view");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
		transInvModelViewLocation = getLocation("transInvModelView");
	}


	void SceneShader::setViewMatrix(const glm::mat4& view) {
		this->view = &view;
	}

	void SceneShader::bindViewMatrix() const {
		if (view != nullptr)
			bind<glm::mat4>(viewLocation, *view);
	}

	void SceneShader::bindMatrices(const glm::mat4& model) const {
		if (view != nullptr) {
			glm::mat4& modelView = (*view) * model;
			glm::mat3& transInvModelView = glm::transpose(glm::inverse(glm::mat3(modelView)));

			bind<glm::mat4>(modelViewLocation, modelView);
			bind<glm::mat3>(transInvModelViewLocation, transInvModelView);
		}
	}

}