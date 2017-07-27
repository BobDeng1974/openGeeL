#ifndef SHADERPROVIDER_H
#define SHADERPROVIDER_H

#include <map>
#include <string>
#include "../utility/properties.h"

namespace geeL {

	class RenderScene;
	class Shader;
	class WorldMapProvider;


	struct ResourceInfo {

	public:
		bool exists;
		std::string type;

		ResourceInfo(bool exists, const std::string& type) : exists(exists), type(type) {}

	};


	//Provider class that can bind and update scene information into requesting shaders
	class ShaderProvider {

	public:
		ShaderProvider(RenderScene& scene, WorldMapProvider& mapProvider);

		ResourceInfo linkResource(const std::string& name, Shader& shader);

	private:
		RenderScene& scene;
		WorldMapProvider& mapProvider;

		std::map<std::string, gFloat*> floatProperties;
		std::map<std::string, gInteger*> intProperties;

		std::map<std::string, gVector3*> vec3Properties;
		std::map<std::string, gVector4*> vec4Properties;
		std::map<std::string, gMat4*> mat4Properties;

	};

}

#endif
