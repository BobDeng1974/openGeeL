#include <gtc/matrix_transform.hpp>
#include "sceneshader.h"

using namespace std;

namespace geeL {

	SceneShader::SceneShader() : RenderShader(), shader(""), space(ShaderTransformSpace::View), cameraName("camera"),
		skyboxName("skybox"), view(nullptr), model(nullptr) {}

	SceneShader::SceneShader(const std::string& vertexPath, const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, string cameraName, string skyboxName, ShaderProvider* const provider)
			: RenderShader(vertexPath.c_str(), fragmentPath.path.c_str(), provider), shader(fragmentPath), space(space), 
				cameraName(cameraName), skyboxName(skyboxName), view(nullptr), model(nullptr) {

		viewLocation = getLocation("viewMatrix");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
		transInvModelViewLocation = getLocation("transInvModelView");
	}

	SceneShader::SceneShader(const std::string& vertexPath, const std::string& geometryPath, const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, string cameraName, string skyboxName, ShaderProvider* const provider)
			: RenderShader(vertexPath.c_str(), geometryPath.c_str(), fragmentPath.path.c_str(), provider), shader(fragmentPath), 
				space(space), cameraName(cameraName), skyboxName(skyboxName), view(nullptr), model(nullptr) {

		viewLocation = getLocation("viewMatrix");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
		transInvModelViewLocation = getLocation("transInvModelView");
	}


	void SceneShader::setViewMatrix(const glm::mat4& view) {
		this->view = &view;
	}

	void SceneShader::setModelMatrix(const glm::mat4& model) {
		this->model = &model;
	}

	void SceneShader::bindViewMatrix() const {
		if (view != nullptr)
			bind<glm::mat4>(viewLocation, *view);
	}

	void SceneShader::bindModel() const {
		if (model != nullptr)
			bind<glm::mat4>(modelLocation, *model);
	}

	void SceneShader::bindMatrices() const {
		if (view != nullptr && model != nullptr) {
			glm::mat4& modelView = (*view) * (*model);
			glm::mat3& transInvModelView = glm::transpose(glm::inverse(glm::mat3(modelView)));

			bind<glm::mat4>(modelViewLocation, modelView);
			bind<glm::mat3>(transInvModelViewLocation, transInvModelView);
		}
		//Only bind model if no view matrix was attached
		else if(model != nullptr)
			bind<glm::mat4>("model", *model);

	}

}