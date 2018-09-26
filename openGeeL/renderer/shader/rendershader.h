#ifndef RENDERSHADER_H
#define RENDERSHADER_H

#include <functional>
#include <list>
#include "shader.h"

namespace geeL {

	class  Material;
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


		virtual bool addMap(const ITexture& texture, const std::string& name);
		virtual std::string removeMap(const ITexture& texture);
		virtual bool removeMap(const std::string& name);

		//Collects draw calls and executes them in packs. 
		//Note: 'forceDraw' should be called after last call, to make sure that remaining draw calls
		//get executed as well
		void draw(const Material& material, std::function<void(const Material&)> drawCall);

		//Force all currently collected draw calls to execute. 
		void forceDraw();

	protected:
		void init(const char* vertexPath, const char* geometryPath, const char* fragmentPath, 
			ShaderProvider* const provider, std::list<const StringReplacement*>& replacements);

	private:
		struct Drawcall {
			const Material& material; 
			std::function<void(const Material&)> drawCall;

			Drawcall(const Material& material, std::function<void(const Material&)> drawCall)
				: material(material), drawCall(drawCall) {}

		};

		std::list<Drawcall> drawcalls;
		unsigned int queueSize;


		void drawPack(unsigned int offset);

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
			: Shader()
			, queueSize(0) {

		std::list<const StringReplacement*> list;
		add(list, replacements...);

		name = fragmentPath;
		init(vertexPath, geometryPath, fragmentPath, provider, list);
	}

}

#endif

