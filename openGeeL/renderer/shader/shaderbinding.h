#ifndef SHADERBINDING_H
#define SHADERBINDING_H

#include <string>
#include "../utility/range.h"

typedef int ShaderLocation;

namespace geeL {

	class Shader;
	class Texture;

	//Base class for all parameters that can be bound into a shader
	class ShaderBinding {

	public:
		ShaderBinding(const Shader& shader, const std::string&name);

		virtual void bind() const = 0;
		virtual std::string toString() const;

	protected:
		std::string name;
		const Shader& shader;
		ShaderLocation location;

	};


	template<class T>
	class ReferenceBinding : public ShaderBinding {

	public:
		ReferenceBinding(const Shader& shader, const std::string&name, const T& value);

		virtual void bind() const;

	protected:
		const T& value;

	};


	template<class T>
	class GenericBinding : public ShaderBinding {

	public:
		GenericBinding(const Shader& shader, const std::string&name, const T& value);

		virtual void bind() const;

		virtual const T& getValue() const;
		virtual bool setValue(const T& value);

	protected:
		T value;

	};


	template<class T>
	class ComparableBinding : public GenericBinding<T> {

	public:
		ComparableBinding(const Shader& shader, const std::string&name, const T& value, Range<T> range);

		virtual bool setValue(const T& value);

		const Range<T>& getRange() const;

	private:
		Range<T> range;

	};


	struct TextureBinding {
		const Texture* texture;
		unsigned int offset;
		std::string name;

		TextureBinding() {}

		TextureBinding(const Texture* texture, unsigned offset, std::string name) :
			texture(texture), offset(offset), name(name) {}

		bool operator== (const TextureBinding& rhs);

	};



	template<class T>
	inline ReferenceBinding<T>::ReferenceBinding(const Shader& shader, const std::string& name, const T& value)
		: ShaderBinding(shader, name), value(value) {}

	template<class T>
	inline void ReferenceBinding<T>::bind() const {
		shader.bind<T>(location, value);
	}



	template<class T>
	inline GenericBinding<T>::GenericBinding(const Shader& shader, const std::string& name, const T& value)
		: ShaderBinding(shader, name), value(value) {}

	template<class T>
	inline void GenericBinding<T>::bind() const {
		shader.bind<T>(location, value);
	}

	template<class T>
	inline const T& GenericBinding<T>::getValue() const {
		return value;
	}

	template<class T>
	inline bool GenericBinding<T>::setValue(const T& value) {
		this->value = value;
		return true;
	}



	template<class T>
	inline ComparableBinding<T>::ComparableBinding(const Shader& shader, const std::string& name, const T& value, Range<T> range)
		: GenericBinding<T>(shader, name, range.clamp(value)), range(range) {}


	template<class T>
	inline bool ComparableBinding<T>::setValue(const T& value) {
		if (this->value != value && range.inside(value)) {
			this->value = value;

			return true;
		}

		return false;
	}

	template<class T>
	inline const Range<T>& ComparableBinding<T>::getRange() const {
		return range;
	}


}

#endif