#ifndef GENERICMATERIAL_H
#define GENERICMATERIAL_H

#include <utility>
#include <map>
#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../texturing/layeredtexture.h"
#include "material.h"

using glm::vec3;
using glm::mat4;

namespace geeL {

	class TextureMap;
	class SceneShader;

	//A generic material class that allow a wide array of parameters
	class GenericMaterialContainer : public MaterialContainer {

	public:
		GenericMaterialContainer(std::string name = "material");

		void addTexture(std::string name, TextureMap& texture);
		void addTextures(std::vector<TextureMap*> textures);

		void addParameter(std::string name, float parameter);
		void addParameter(std::string name, int parameter);
		void addParameter(std::string name, vec3 parameter);
		void addParameter(std::string name, mat4 parameter);

		void bindTextures(SceneShader& shader) const;
		void bind(SceneShader& shader) const;

		virtual float getFloatValue(std::string name) const;
		virtual int   getIntValue(std::string name) const;
		virtual vec3  getVectorValue(std::string name) const;
		virtual mat4  getMatrixValue(std::string name) const;

		virtual void setFloatValue(std::string name, float value) ;
		virtual void setIntValue(std::string name, int value);
		virtual void setVectorValue(std::string name, const glm::vec3& value);
		virtual void setMatrixValue(std::string name, mat4 value);


	private:
		LayeredTexture textureStack;

		std::map<std::string, float> floatParameters;
		std::map<std::string, int> intParameters;
		std::map<std::string, vec3> vec3Parameters;
		std::map<std::string, mat4> mat4Parameters;

	};
}

#endif
