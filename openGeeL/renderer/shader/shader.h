#ifndef SHADER_H
#define SHADER_H

#include <map>
#include <list>
#include <functional>
#include <queue>
#include <string>
#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "shaderbinding.h"
#include "texturing/texturetype.h"
#include "utility/properties.h"
#include "utility/defaults.h"
#include "bindingstack.h"

typedef int ShaderLocation;

using FloatBinding = geeL::ComparableBinding<float>;
using IntegerBinding = geeL::ComparableBinding<int>;

namespace geeL {

	class ITexture;


	//Container class that creates and forwards information to linked GLSL shaders
	class Shader {

	public:
		unsigned int mapBindingPos;
		std::string name;

		virtual ~Shader();
		
		unsigned int getProgram() const;

		//Add a new map as texture sampler to the shader
		void addMap(const ITexture& texture, const std::string& name);
		const ITexture* const getMap(const std::string& name) const;

		//Remove map from shader (if it is attached) and returns its name
		virtual std::string removeMap(const ITexture& texture);

		//Remove map with given name from shader (if it exists)
		virtual void removeMap(const std::string& name);

		//Load associated texture samplers into their binding points in the shader
		virtual void loadMaps() const;

		//Load associated texture samplers into their binding points 
		//in the shader with some internal optimizations.
		//Note: Should only be used if texture repetition is
		//expected in concurrent calls since some overhead is
		//to be expected
		virtual void loadMapsDynamic() const;

		//Init function that should be called before draw calls. It a
		//ctivates shader program and loads all necessary maps
		virtual void initDraw() const;

		ShaderLocation getLocation(const std::string& name) const;

		//Bind value into shader. Value won't be saved in this 
		//shader class and can't be accessed later on.
		//Valid types: bool, int, uint, float, vec2, vec3, vec4, mat3, mat4
		template<class T>
		ShaderLocation bind(const std::string& name, const T& value) const;

		template<class T>
		ShaderLocation bind(const std::string& name, const PropertyBase<T>& value) const;
		
		//Bind value into shader. Value won't be saved in this 
		//shader class and can't be accessed later on.
		//Valid types: bool, int, uint, float vec2, vec3, vec4, mat3, mat4
		template<class T>
		void bind(ShaderLocation location, const T& value) const;

		template<class T>
		void bind(ShaderLocation location, const PropertyBase<T>& value) const;

		//Assign given property to be monitored by this shader and automatically
		//updated/bound into shader program. Needs to be called only once
		template<class T>
		void monitorValue(const std::string& name, PropertyBase<T>& value);

		/*
		//Assign given nested property to be monitored by this shader and automatically
		//updated/bound into shader program. Needs to be called only once
		template<class T, class V>
		void monitorValue(const std::string& name, NestedProperty1x<T, V>& value);

		//Assign given nested property to be monitored by this shader and automatically
		//updated/bound into shader program. Needs to be called only once
		template<class T, class U, class V>
		void monitorValue(const std::string& name, NestedProperty2x<T, U, V>& value);
		*/

		//Set and save value into this shader. Values can be accessed and will be bound
		//by 'bindParameters' call
		//Valid types: int, float, vec2, vec3, vec4, mat3, mat4
		template<class T>
		void setValue(const std::string& name, const T& value);

		//Set and save value into this shader. Values can be accessed and will be bound
		//by 'bindParameters' call. All following calls will clamp value into given range
		//Valid types: int, float
		template<class T>
		void setValue(const std::string& name, const T& value, Range<T> range);

		//Access value by name (if it exists)
		template<class T>
		const T& getValue(const std::string& name) const;


		void setValue(const std::string& name, float value, Range<float> range = UNLIMITED_FLOAT_RANGE);
		void setValue(const std::string& name, int value, Range<int> range = UNLIMITED_INT_RANGE);
		
		float getFloatValue(const std::string& name) const;
		int getIntValue(const std::string& name) const;

		void bindParameters();

		void iterateBindings(std::function<void(const ShaderBinding&)> function);
		void iterateTextures(std::function<void(const TextureBinding& binding)> function) const;

		void setMapOffset(unsigned int value);

	protected:
		unsigned int program;
		std::map<std::string, TextureBinding> maps;
		std::map<std::string, ShaderBinding*> shaderBindings;

		std::queue<ShaderBinding*> bindingQueue;
		std::queue<ShaderBinding*> tempQueue;

		Shader() : mapBindingPos(0) {}

		void use() const;

		ShaderLocation bind(const std::string& name, bool value) const;
		ShaderLocation bind(const std::string& name, int value) const;
		ShaderLocation bind(const std::string& name, unsigned int value) const;
		ShaderLocation bind(const std::string& name, float value) const;
		ShaderLocation bind(const std::string& name, const glm::vec2& value) const;
		ShaderLocation bind(const std::string& name, const glm::vec3& value) const;
		ShaderLocation bind(const std::string& name, const glm::vec4& value) const;
		ShaderLocation bind(const std::string& name, const glm::mat3& value) const;
		ShaderLocation bind(const std::string& name, const glm::mat4& value) const;

		ShaderLocation bind(const std::string& name, const gvec2& value) const;
		ShaderLocation bind(const std::string& name, const gvec3& value) const;
		ShaderLocation bind(const std::string& name, const gvec4& value) const;
		ShaderLocation bind(const std::string& name, const gmat3& value) const;
		ShaderLocation bind(const std::string& name, const gmat4& value) const;

		void bind(ShaderLocation location, const bool& value) const;
		void bind(ShaderLocation location, const int& value) const;
		void bind(ShaderLocation location, const unsigned int& value) const;
		void bind(ShaderLocation location, const float& value) const;
		void bind(ShaderLocation location, const glm::vec2& value) const;
		void bind(ShaderLocation location, const glm::vec3& value) const;
		void bind(ShaderLocation location, const glm::vec4& value) const;
		void bind(ShaderLocation location, const glm::mat3& value) const;
		void bind(ShaderLocation location, const glm::mat4& value) const;

		void bind(ShaderLocation location, const gvec2& value) const;
		void bind(ShaderLocation location, const gvec3& value) const;
		void bind(ShaderLocation location, const gvec4& value) const;
		void bind(ShaderLocation location, const gmat3& value) const;
		void bind(ShaderLocation location, const gmat4& value) const;

		template<class T>
		std::function<void(const T&)> createCallback(const std::string& name);

	private:
		static unsigned int activeProgram;
		unsigned int mapOffset = 0;

		//Bind all added maps into the shader
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
	inline ShaderLocation Shader::bind(const std::string& name, const PropertyBase<T>& value) const {
		const T& val = value;

		return bind<T>(name, val);
	}

	template<class T>
	inline void Shader::bind(ShaderLocation location, const T& value) const {
		use();
		bind(location, value);
	}

	template<class T>
	inline void Shader::bind(ShaderLocation location, const PropertyBase<T>& value) const {
		const T& val = value;
		bind<T>(location, val);
	}

	template<class T>
	inline void Shader::monitorValue(const std::string& name, PropertyBase<T>& value) {
		value.addListener(createCallback<T>(name), true);
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
	inline const T& Shader::getValue(const std::string& name) const {
		auto it = shaderBindings.find(name);
		if (it != shaderBindings.end()) {
			GenericBinding<T>* binding = static_cast<GenericBinding<T>*>(it->second);

			return binding->getValue();
		}

		return DefaultValues::get<T>();
	}

	template<class T>
	inline std::function<void(const T&)> Shader::createCallback(const std::string& name) {
		return [this, &name](const T& t) {
			ReferenceBinding<T>* binding = new ReferenceBinding<T>(*this, name, t);
			tempQueue.push(binding);
		};
	}

}

#endif


