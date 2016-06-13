#include <list>
#include "shader.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../materials/materialfactory.h"
#include "shadermanager.h"

namespace geeL {

	ShaderManager::ShaderManager(const MaterialFactory& factory) 
		: factory(factory) {}


	void ShaderManager::staticBind(const LightManager& lightManager) const {

		for (list<Shader>::const_iterator it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			const Shader& shader = *it;

			shader.use();
			if (shader.useLight) lightManager.bind(shader);
		}
	}

	void ShaderManager::dynamicBind(const LightManager& lightManager, const Camera& currentCamera, string cameraName) const {

		for (list<Shader>::const_iterator it = factory.shadersBegin(); it != factory.shadersEnd(); it++) {
			const Shader& shader = *it;

			shader.use();
			if (shader.useLight) lightManager.bind(shader);
			if (shader.useCamera) currentCamera.bind(shader, cameraName);
		}
	}


}