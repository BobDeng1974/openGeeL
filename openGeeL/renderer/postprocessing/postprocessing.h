#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <list>
#include <string>
#include "renderer/glstructures.h"
#include "shader/rendershader.h"
#include "shader/computeshader.h"
#include "utility/resolution.h"
#include "glwrapper/masking.h"

namespace geeL {

	class ColorBuffer;
	class DynamicBuffer;
	class PostProcessingEffect;
	class RenderShader;
	class ScreenQuad;
	class Texture;
	class ITextureProvider;
	class ITexture;
	class MaterialFactory;


	struct PostProcessingParameter {
		ScreenQuad& screen;
		DynamicBuffer& buffer;
		DynamicBuffer* separatedBuffer;
		Resolution resolution;
		PostProcessingEffect* fallbackEffect;
		ITextureProvider* provider;
		MaterialFactory* factory;

		PostProcessingParameter(ScreenQuad& screen, 
			DynamicBuffer& buffer, 
			const Resolution& resolution, 
			ITextureProvider* const provider = nullptr,
			PostProcessingEffect* const fallbackEffect = nullptr,
			DynamicBuffer* separatedBuffer = nullptr,
			MaterialFactory* factory = nullptr);

		PostProcessingParameter(const PostProcessingParameter& other, const Resolution& resolution);

	};


	//Interface for all post processing effects (or rendering passes) that can enhance an existing image
	class PostProcessingEffect : public Drawer {

	public:
		virtual ~PostProcessingEffect() {}

		//Returns main image of this effect
		virtual const ITexture& getImage() const = 0;

		//Set main image buffer as texture sampler that will be used as base for post processing
		virtual void setImage(const ITexture& texture) = 0;
		virtual void addTextureSampler(const ITexture& texture, const std::string& name) = 0;

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues() = 0;
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

		bool isActive() const;
		virtual void setActive(bool value);

		virtual Shader& getShader() = 0;

	protected:
		ITextureProvider* provider = nullptr;
		Resolution resolution;
		bool onlyEffect = false;
		bool active = true;

	};


	//Post processing effect that gets drawn via fragment shader during a classical rendering pipeline call
	class PostProcessingEffectFS : public PostProcessingEffect {

	public:
		PostProcessingEffectFS(const std::string& fragmentPath);
		PostProcessingEffectFS(const std::string& vertexPath, const std::string& fragmentPath);

		template<typename ...Replacements>
		PostProcessingEffectFS(const std::string& vertexPath, const std::string& fragmentPath,
			const Replacements& ...replacements);

		virtual ~PostProcessingEffectFS() {}

		virtual const ITexture& getImage() const;
		virtual void setImage(const ITexture& texture);
		virtual void addTextureSampler(const ITexture& texture, const std::string& name);

		//Add a render mask to this effect. The effect will then only 
		//be drawn in regions that have been marked with given mask
		//Note: Multiple map operation is AND (not OR)
		virtual void setRenderMask(RenderMask mask);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues() {}
		virtual void draw();
		virtual void fill();

		virtual Shader& getShader();

		virtual std::string toString() const;

	protected:
		static constexpr const char* defaultVertexPath = "shaders/screen.vert";

		RenderMask mask = RenderMask::None;
		RenderShader shader;
		ScreenQuad* screen;
		PostProcessingEffect* fallbackEffect = nullptr;
		
		virtual void drawSubImages() {}
		virtual void bindToScreen();
		
	};


	//Post processing effect that gets drawn via a compute shader
	class PostProcessingEffectCS : public PostProcessingEffect {

	public:
		PostProcessingEffectCS(const std::string& path, Resolution groupSize = Resolution(8, 8));
		virtual ~PostProcessingEffectCS() {}

		virtual const ITexture& getImage() const;
		virtual void setImage(const ITexture& texture);
		virtual void addTextureSampler(const ITexture& texture, const std::string& name);
		virtual void addImageTexture(const ITexture& texture, unsigned int bindingPosition);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues() {}
		virtual void draw();
		virtual void fill();

		virtual Shader& getShader();

		virtual std::string toString() const;

	protected:
		ComputeShader shader;
		Resolution groupSize;

		//Gets called when effects gets drawn/filled to load target of operation
		//By default, the current image (from render pipeline) is set as target
		//and its resolution bound 
		virtual void bindTextureTargets();

	};


	//Post proccessing effect that simply outputs the input image
	class PassthroughEffect : public PostProcessingEffectFS {

	public:
		PassthroughEffect() 
			: PostProcessingEffectFS("shaders/postprocessing/passthrough.frag") {}

	};



	inline PostProcessingParameter::PostProcessingParameter(ScreenQuad& screen, 
		DynamicBuffer& buffer, 
		const Resolution& resolution,
		ITextureProvider* const provider,
		PostProcessingEffect* const fallbackEffect,
		DynamicBuffer* separatedBuffer,
		MaterialFactory* factory)
			: screen(screen)
			, buffer(buffer)
			, separatedBuffer(separatedBuffer)
			, resolution(resolution)
			, fallbackEffect(fallbackEffect)
			, provider(provider)
			, factory(factory) {}

	inline PostProcessingParameter::PostProcessingParameter(const PostProcessingParameter& other, 
		const Resolution& resolution) 
			: screen(other.screen)
			, buffer(other.buffer)
			, separatedBuffer(other.separatedBuffer)
			, resolution(resolution)
			, fallbackEffect(other.fallbackEffect)
			, provider(other.provider)
			, factory(other.factory) {}

	
	template<typename ...Replacements>
	inline PostProcessingEffectFS::PostProcessingEffectFS(
		const std::string& vertexPath,
		const std::string& fragmentPath, 
		const Replacements& ...replacements) 
			: shader(RenderShader(vertexPath.c_str(),
				fragmentPath.c_str(), 
				nullptr, 
				nullptr, 
				replacements...)) {}
				
}

#endif
