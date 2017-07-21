#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <list>
#include <queue>
#include <string>
#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "shaderbinding.h"
#include "../texturing/texturetype.h"

typedef int ShaderLocation;

using FloatBinding = geeL::ComparableBinding<float>;
using IntegerBinding = geeL::ComparableBinding<int>;

namespace geeL {

	class Texture;


	//Container class that creates and forwards information to linked GLSL shaders
	class Shader {

	public:
		unsigned int mapBindingPos;
		unsigned int mapOffset = 0;
		std::string name;

		~Shader();
		
		unsigned int getProgram() const;

		//Add a new map to the shader
		void addMap(const Texture& texture, const std::string& name);
		const Texture * const getMap(const std::string& name) const;

		//Remove map from shader (if it is attached)
		void removeMap(const Texture& texture);

		//Remove map with given name from shader (if it exists)
		void removeMap(const std::string& name);

		//Load maps into their binding points in the shader
		virtual void loadMaps() const;

		//Loads committed maps into the shader
		//IMPORTANT: no binding is taken care of, multiple calls will override previous one
		//and loading maps from other sources will also override this call
		void loadMaps(std::list<unsigned int>& maps, TextureType type = TextureType::Texture2D) const;

		ShaderLocation getLocation(const std::string& name) const;

		//Bind value into shader. Value won't be saved in this 
		//shader class and can't be accessed later on.
		//Valid types: int, float vec2, vec3, vec4, mat3, mat4
		template<class T>
		ShaderLocation bind(const std::string& name, const T& value) const;
		
		//Bind value into shader. Value won't be saved in this 
		//shader class and can't be accessed later on.
		//Valid types: int, float vec2, vec3, vec4, mat3, mat4
		template<class T>
		void bind(ShaderLocation location, const T& value) const;
		
		//Set and save value into this shader. Values can be accessed and will be bound
		//by 'bindParameters' call
		//Valid types: int, float vec2, vec3, vec4, mat3, mat4
		template<class T>
		void setValue(const std::string& name, const T& value);

		//Set and save value into this shader. Values can be accessed and will be bound
		//by 'bindParameters' call. All following calls will clamp value into given range
		//Valid types: int, float
		template<class T>
		void setValue(const std::string& name, const T& value, Range<T> range);

		//Access value by name (if it exists)
		template<class T>
		T getValue(const std::string& name) const;


		void setValue(const std::string& name, float value, Range<float> range = UNLIMITED_FLOAT_RANGE);
		void setValue(const std::string& name, int value, Range<int> range = UNLIMITED_INT_RANGE);
		
		float getFloatValue(const std::string& name) const;
		int getIntValue(const std::string& name) const;

		void bindParameters();

	protected:
		unsigned int program;
		std::map<std::string, TextureBinding> maps;
		std::map<std::string, ShaderBinding*> shaderBindings;

		std::queue<ShaderBinding*> bindingQueue;

		Shader() : mapBindingPos(0) {}

		void use() const;

		ShaderLocation bind(const std::string& name, int value) const;
		ShaderLocation bind(const std::string& name, float value) const;
		ShaderLocation bind(const std::string& name, const glm::vec2& value) const;
		ShaderLocation bind(const std::string& name, const glm::vec3& value) const;
		ShaderLocation bind(const std::string& name, const glm::vec4& value) const;
		ShaderLocation bind(const std::string& name, const glm::mat3& value) const;
		ShaderLocation bind(const std::string& name, const glm::mat4& value) const;

		void bind(ShaderLocation location, const int& value) const;
		void bind(ShaderLocation location, const float& value) const;
		void bind(ShaderLocation location, const glm::vec2& value) const;
		void bind(ShaderLocation location, const glm::vec3& value) const;
		void bind(ShaderLocation location, const glm::vec4& value) const;
		void bind(ShaderLocation location, const glm::mat3& value) const;
		void bind(ShaderLocation location, const glm::mat4& value) const;

	private:
		//Bind all added maps to the shader
		void bindMaps();

	};


	inline unsigned int Shader::getProgram() const {
		return program;
	}


	template<class T>
	inline ShaderLocation Shader::bind(const std::string& name, const T& value) const {
		use();
		return bind(name, value);
	}

	template<class T>
	inline void Shader::bind(ShaderLocation location, const T& value) const {
		use();
		bind(location, value);
	}

	template<class T>
	inline void Shader::setValue(const std::string& name, const T& value) {
		GenericBinding<T>* binding;

		auto it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			binding = static_cast<GenericBinding<T>*>(it->second);

			bool set = binding->setValue(value);
			if (set) bindingQueue.push(binding);
		}
		else {
			binding = new GenericBinding<T>(*this, name, value);
			shaderBindings[name] = binding;

			bindingQueue.push(binding);
		}
	}

	template<class T>
	inline void Shader::setValue(const std::string& name, const T& value, Range<T> range) {
		ComparableBinding<T>* binding;

		auto it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			binding = static_cast<ComparableBinding<T>*>(it->second);

			bool set = binding->setValue(value);
			if (set) bindingQueue.push(binding);
		}
		else {
			binding = new ComparableBinding<T>(*this, name, value, range);
			shaderBindings[name] = binding;

			bindingQueue.push(binding);
		}
	}

	template<class T>
	inline T Shader::getValue(const std::string& name) const {
		auto it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			GenericBinding<T>* binding = static_cast<GenericBinding<T>*>(it->second);

			return binding->getValue();
		}

		return T();
	}

}

#endif


