#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "materials/material.h"
#include "bindingstack.h"
#include "shaderreader.h"
#include "rendershader.h"

using namespace std;

namespace geeL {
	
	RenderShader::RenderShader()
		: Shader()
		, queueSize(0) {}

	RenderShader::RenderShader(const char* vertexPath, const char* fragmentPath,
		const char* geometryPath, ShaderProvider* const provider) 
			: Shader()
			, queueSize(0) {

		list<const StringReplacement*> list;

		name = fragmentPath;
		init(vertexPath, geometryPath, fragmentPath, provider, list);
	}


	bool RenderShader::addMap(const ITexture& texture, const string& name) {
		bool added = Shader::addMap(texture, name);
		if (added) queueSize++;

		return added;
	}

	string RenderShader::removeMap(const ITexture& texture) {
		string& name = Shader::removeMap(texture);

		//No empty string returned => map successfully removed
		if (name.size() > 0) queueSize--;

		return name;
	}

	bool RenderShader::removeMap(const string& name) {
		bool removed = Shader::removeMap(name);
		if (removed) queueSize--;

		return removed;
	}

	void RenderShader::draw(const Material& material, function<void(const Material&)> drawCall) {
		const MaterialContainer& container = material.getMaterialContainer();
		unsigned int size = container.getTextureCount();

		//Add draw call to queue until binding stack is full
		if ((queueSize + size) < TextureBindingStack::MAX_TEXTURE_ACTIVE)
			queueSize += size;
		else 
			drawPack(size);


		//Add current draw call to "next" call stack
		drawcalls.push_back(Drawcall(material, drawCall));
	}

	void RenderShader::forceDraw() {
		drawPack(0);
	}

	void RenderShader::drawPack(unsigned int offset) {
		queueSize = getMapCount() + offset;

		//Add all maps present in shader
		loadMaps();

		//Activate all textures of all operations on texture stack
		list<list<TextureBinding>> bindings;
		for (auto it(drawcalls.begin()); it != drawcalls.end(); it++) {
			Drawcall& call = *it;
			const MaterialContainer& m = call.material.getMaterialContainer();

			bindings.push_back(list<TextureBinding>());

			m.iterTextures([&](const string& name, const ITexture& texture) {
				StackPosition p = TextureBindingStack::activateTexture(texture);

				list<TextureBinding>& l = bindings.back();
				l.push_back(TextureBinding(&texture, p, name));
			});
		}


		//Perform all previously collected draw calls in order
		auto bIt(bindings.begin());
		for (auto it(drawcalls.begin()); it != drawcalls.end(); it++) {
			Drawcall& call = *it;
			const Material& m = call.material;

			//Bind all textures that are associated with current draw call
			list<TextureBinding>& b = *bIt;
			for (auto et(b.begin()); et != b.end(); et++) {
				TextureBinding& binding = *et;

				bind<unsigned int>(binding.name, binding.offset);
			}

			call.drawCall(m);
		}

		drawcalls.clear();
	}


	void RenderShader::init(const char* vertexPath, const char* geometryPath, const char* fragmentPath,
		ShaderProvider* const provider, list<const StringReplacement*>& replacements) {

		//Vertex shader
		string vertexCode = FileReader::readFile(vertexPath);
		ShaderFileReader::preprocessShaderString(*this, vertexCode, vertexPath, provider);
		const GLchar* vShaderCode = vertexCode.c_str();

		GLuint vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
		}


		//Fragment shader
		string fragmentCode = FileReader::readFile(fragmentPath);
		ShaderFileReader::preprocessShaderString(*this, fragmentCode, fragmentPath, provider);

		ShaderFileReader::setGlobalVariables(fragmentCode);
		for (auto it(replacements.begin()); it != replacements.end(); it++)
			FileReader::replaceOccurence(fragmentCode, **it);


		const GLchar* fShaderCode = fragmentCode.c_str();

		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
		}

		//Geometry shader
		GLuint geometryShader;
		if (geometryPath != nullptr) {
			string geometryCode = FileReader::readFile(geometryPath);
			ShaderFileReader::preprocessShaderString(*this, geometryCode, geometryPath, provider);
			const GLchar* gShaderCode = geometryCode.c_str();

			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

			glShaderSource(geometryShader, 1, &gShaderCode, NULL);
			glCompileShader(geometryShader);

			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
				cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
			}
		}
		

		//Put progam together
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		if (geometryPath != nullptr) glAttachShader(program, geometryShader);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (geometryPath != nullptr) glDeleteShader(geometryShader);
	}

}

