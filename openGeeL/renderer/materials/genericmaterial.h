#ifndef GENERICMATERIAL_H
#define GENERICMATERIAL_H

#include <utility>
#include <map>
#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "material.h"

using glm::vec3;
using glm::mat4;

namespace geeL {

	class TextureMap;
	class SceneShader;

	//A generic material class that allow a wide array of parameters
	class GenericMaterialContainer : public MaterialContainer {

	public:
		GenericMaterialContainer(const std::string& name = "material");

		//Add a new texture unit with given name. Shader will be notified
		//if unit is filled or not during rendering process via variable 
		//$(name)Bound. (E.g. unit diffuse bound => diffuseBound = true) 
		virtual void addTextureUnit(const std::string& name);

		//Add a new texture to material and fill the texture unit that has given name
		//(or create a new one if such unit doens't exist)
		virtual void addTexture(const std::string& name, MemoryObject<ITexture> texture);

		void addParameter(const std::string& name, float parameter);
		void addParameter(const std::string& name, int parameter);
		void addParameter(const std::string& name, vec3 parameter);
		void addParameter(const std::string& name, mat4 parameter);

		void bind(SceneShader& shader) const;

		virtual float getFloatValue(const std::string& name) const;
		virtual int   getIntValue(const std::string& name) const;
		virtual vec3  getVectorValue(const std::string& name) const;
		virtual mat4  getMatrixValue(const std::string& name) const;

		virtual void setFloatValue(const std::string&, float value) ;
		virtual void setIntValue(const std::string&, int value);
		virtual void setVectorValue(const std::string&, const glm::vec3& value);
		virtual void setMatrixValue(const std::string&, mat4 value);


	private:
		std::map<std::string, MemoryObject<ITexture>> textures;
		std::map<std::string, float> floatParameters;
		std::map<std::string, int> intParameters;
		std::map<std::string, vec3> vec3Parameters;
		std::map<std::string, mat4> mat4Parameters;

	};
}

#endif
