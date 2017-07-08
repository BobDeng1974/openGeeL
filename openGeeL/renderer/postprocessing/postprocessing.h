#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <list>
#include <string>
#include "../renderer.h"
#include "../shader/rendershader.h"

typedef unsigned int GLuint;

namespace geeL {

	struct FrameBufferInformation;
	class FrameBuffer;
	class ColorBuffer;
	class RenderShader;
	class ScreenQuad;
	class Texture;

	class PostProcessingEffect : public Drawer {

	public:
		PostProcessingEffect(std::string fragmentPath);
		PostProcessingEffect(std::string vertexPath, std::string fragmentPath);

		void setScreen(ScreenQuad& screen);

		//Returns first (and main) buffer of this effect
		const Texture& getBuffer() const;

		//Set main image buffer that will be used as base for post processing
		void setBuffer(const ColorBuffer& buffer);
		virtual void setBuffer(const Texture& texture);
		void addBuffer(const Texture& texture, const std::string& name);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void draw();

		virtual std::string toString() const;

		//Determine whether to draw only the effect or effect embed into scene.
		//May not result in different rendering depending on effect;
		virtual void effectOnly(bool only);

		//Check if effect is currently set for drawing effect only or not
		virtual bool getEffectOnly() const;

	protected:
		RenderShader shader;
		ScreenQuad* screen;
		bool onlyEffect;

		virtual void bindValues() {}
		virtual void bindToScreen();
	};


	//Post proccessing effect that simply outputs the input image
	class PassthroughEffect : public PostProcessingEffect {

	public:
		PassthroughEffect() 
			: PostProcessingEffect("renderer/postprocessing/passthrough.frag") {}

	};

}

#endif
