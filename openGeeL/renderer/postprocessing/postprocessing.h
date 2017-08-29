#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <list>
#include <string>
#include "renderer.h"
#include "shader/rendershader.h"
#include "shader/computeshader.h"
#include "utility/resolution.h"

typedef unsigned int GLuint;

namespace geeL {

	class ColorBuffer;
	class DynamicBuffer;
	class RenderShader;
	class ScreenQuad;
	class Texture;


	//Interface for all post processing effects (or rendering passes) that can enhance an existing image
	class PostProcessingEffect : public Drawer {

	public:
		virtual ~PostProcessingEffect() {}

		//Returns main image of this effect
		virtual const Texture& getImage() const = 0;

		//Set main image buffer as texture sampler that will be used as base for post processing
		virtual void setImage(const Texture& texture) = 0;
		virtual void addTextureSampler(const Texture& texture, const std::string& name) = 0;

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) = 0;
		virtual void draw() = 0;
		virtual void fill() = 0;

		//Determine whether to draw only the effect or effect embed into scene.
		//May not result in different rendering depending on effect;
		void effectOnly(bool only);

		//Check if effect is currently set for drawing effect only or not
		bool getEffectOnly() const;

		virtual std::string toString() const = 0;

		const Resolution& getResolution() const;
		void setResolution(const Resolution& value);

		virtual Shader& getShader() = 0;

	protected:
		Resolution resolution;
		bool onlyEffect = false;

	};


	//Post processing effect that gets drawn via fragment shader during a classical rendering pipeline call
	class PostProcessingEffectFS : public PostProcessingEffect {

	public:
		PostProcessingEffectFS(const std::string& fragmentPath);
		PostProcessingEffectFS(const std::string& vertexPath, const std::string& fragmentPath);
		virtual ~PostProcessingEffectFS() {}

		virtual const Texture& getImage() const;
		virtual void setImage(const Texture& texture);
		virtual void addTextureSampler(const Texture& texture, const std::string& name);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);
		virtual void draw();
		virtual void fill();

		virtual Shader& getShader();

		virtual std::string toString() const;

	protected:
		RenderShader shader;
		ScreenQuad* screen;

		virtual void bindValues() {}
		virtual void bindToScreen();
	};


	//Post processing effect that gets drawn via a compute shader
	class PostProcessingEffectCS : public PostProcessingEffect {

	public:
		PostProcessingEffectCS(const std::string& path, Resolution groupSize = Resolution(8, 8));
		virtual ~PostProcessingEffectCS() {}

		virtual const Texture& getImage() const;
		virtual void setImage(const Texture& texture);
		virtual void addTextureSampler(const Texture& texture, const std::string& name);
		virtual void addImageTexture(const Texture& texture, unsigned int bindingPosition);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);
		virtual void draw();
		virtual void fill();

		virtual Shader& getShader();

		virtual std::string toString() const;

	protected:
		DynamicBuffer* buffer = nullptr;
		ComputeShader shader;
		Resolution groupSize;

		virtual void bindValues() {}

	};


	//Post proccessing effect that simply outputs the input image
	class PassthroughEffect : public PostProcessingEffectFS {

	public:
		PassthroughEffect() 
			: PostProcessingEffectFS("renderer/postprocessing/passthrough.frag") {}

	};

}

#endif
