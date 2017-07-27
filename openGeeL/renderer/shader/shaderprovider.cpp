#include "../utility/worldinformation.h"
#include "../scene.h"
#include "shader.h"
#include "shaderprovider.h"

namespace geeL {

	ShaderProvider::ShaderProvider(RenderScene& scene, WorldMapProvider& mapProvider) 
		: scene(scene), mapProvider(mapProvider) {

		//TODO: add properties to provider. Currently not possible since
		//no class uses properties.
	}

	ResourceInfo ShaderProvider::linkResource(const std::string& name, Shader& shader) {
		
		auto it1(floatProperties.find(name));
		if (it1 != floatProperties.end()) {
			auto& property = *it1->second;
			shader.monitorValue<float>(name, property);

			return ResourceInfo(true, "float");
		}

		auto it2(intProperties.find(name));
		if (it2 != intProperties.end()) {
			auto& property = *it2->second;
			shader.monitorValue<int>(name, property);

			return ResourceInfo(true, "int");
		}

		auto it3(vec3Properties.find(name));
		if (it3 != vec3Properties.end()) {
			auto& property = *it3->second;
			shader.monitorValue<gvec3>(name, property);

			return ResourceInfo(true, "vec3");
		}

		auto it4(vec4Properties.find(name));
		if (it4 != vec4Properties.end()) {
			auto& property = *it4->second;
			shader.monitorValue<gvec4>(name, property);

			return ResourceInfo(true, "vec4");
		}

		auto it5(mat4Properties.find(name));
		if (it5 != mat4Properties.end()) {
			auto& property = *it5->second;
			shader.monitorValue<gmat4>(name, property);

			return ResourceInfo(true, "mat4");
		}
		
		return ResourceInfo(false, "");
	}


}