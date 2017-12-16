#ifndef RENDERSHADER_H
#define RENDERSHADER_H

#include <list>
#include "shader.h"

namespace geeL {

	class  ShaderProvider;
	struct StringReplacement;


	//Shader for the use in OpenGL rendering pipeline
	class RenderShader : public Shader {

	public:
		RenderShader();

		RenderShader(const char* vertexPath, 
			const char* fragmentPath, 
			const char* geometryPath = nullptr,
			ShaderProvider* const provider = nullptr);

		template<typename ...Replacements>
		RenderShader(const char* vertexPath,
			const char* fragmentPath,
			const char* geometryPath,
			ShaderProvider* const provider,
			const Replacements& ...replacements);

	protected:
		void init(const char* vertexPath, const char* geometryPath, const char* fragmentPath, 
			ShaderProvider* const provider, std::list<const StringReplacement*>& replacements);

	};



	inline void add(std::list<const StringReplacement*>& list, const StringReplacement& r) {
		list.push_back(&r);
	}
	
	template<typename ...Replacements>
	inline void add(std::list<const StringReplacement*>& list, const StringReplacement& r, const Replacements& ...rs) {
		add(list, r);
		add(list, rs...);
	}


	template<typename ...Replacements>
	inline RenderShader::RenderShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath,
		ShaderProvider* const provider, const Replacements& ...replacements)
			: Shader() {

		std::list<const StringReplacement*> list;
		add(list, replacements...);

		name = fragmentPath;
		init(vertexPath, geometryPath, fragmentPath, provider, list);
	}

}

#endif

