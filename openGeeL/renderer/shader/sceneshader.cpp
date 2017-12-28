#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "transformation/transform.h"
#include "lights/lightmanager.h"
#include "cameras/camera.h"
#include "sceneshader.h"

using namespace std;

namespace geeL {

	SceneShader::SceneShader() 
		: RenderShader()
		, shader("")
		, space(ShaderTransformSpace::View)
		, animated(false) {}

	SceneShader::SceneShader(const std::string& vertexPath, 
		const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, 
		ShadingMethod shadingMethod, 
		bool animated,
		ShaderProvider* const provider)
			: RenderShader(vertexPath.c_str(), fragmentPath.path.c_str(), nullptr, provider)
			, shader(fragmentPath)
			, space(space)
			, animated(animated)
			, shadingMethod(shadingMethod) {

		viewLocation = getLocation("view");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
		transInvModelViewLocation = getLocation("transInvModelView");
	}

	SceneShader::SceneShader(const std::string& vertexPath, 
		const std::string& geometryPath, 
		const FragmentShader& fragmentPath, 
		ShaderTransformSpace space, 
		ShadingMethod shadingMethod, 
		bool animated,
		ShaderProvider* const provider)
			: RenderShader(vertexPath.c_str(), fragmentPath.path.c_str(), geometryPath.c_str(), provider)
			, shader(fragmentPath)
			, space(space)
			, animated(animated)
			, shadingMethod(shadingMethod) {

		viewLocation = getLocation("view");
		modelLocation = getLocation("model");
		modelViewLocation = getLocation("modelView");
		transInvModelViewLocation = getLocation("transInvModelView");
	}


	void SceneShader::loadSceneInformation(const LightManager& lightManager, const Camera& camera) {
		if (getUseLight()) lightManager.bind(*this, &camera);
		if (getUseCamera()) setCamera(camera);
	}

	void SceneShader::loadSceneInformation(const LightManager& lightManager, const Camera* camera) {
		if (getUseLight()) lightManager.bind(*this);
		if (camera != nullptr && getUseCamera()) setCamera(*camera);

	}

	void SceneShader::setCamera(const Camera& camera) {
		setViewMatrix(camera.getViewMatrix());

		switch (getMethod()) {
			case ShadingMethod::Hybrid:
			case ShadingMethod::Transparent:
				camera.bindProjectionMatrix(*this, "projection");
				camera.bindInverseViewMatrix(*this, "inverseView");
				bind<glm::vec3>("origin", camera.GetOriginInViewSpace());

				if (isAnimated())
					bindViewMatrix();

				break;
			case ShadingMethod::Forward:
				camera.bindProjectionMatrix(*this, "projection");
				bind<glm::vec3>("cameraPosition", camera.transform.getPosition());

				if (isAnimated())
					bindViewMatrix();

				break;
			}
	}

	void SceneShader::setViewMatrix(const glm::mat4& view) {
		this->view = view;
	}

	void SceneShader::bindViewMatrix() const {
		bind<glm::mat4>(viewLocation, view);
	}

	void SceneShader::bindMatrices(const Transform& model) const {
		if (space == ShaderTransformSpace::World) {
			model.bind(*this, "model");
			bindViewMatrix();
		}
		else {
			glm::mat4& modelView = (view)* model.getMatrix();
			glm::mat3& transInvModelView = glm::transpose(glm::inverse(glm::mat3(modelView)));

			bind<glm::mat4>(modelViewLocation, modelView);
			bind<glm::mat3>(transInvModelViewLocation, transInvModelView);
		}
			
	}

}