#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

#include <glm.hpp>
#include <string>
#include "../utility/worldinformation.h"
#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	class Camera;
	class GaussianBlur;

	//Simple motion blur that blurs in (inverse) direction of camera movement
	class MotionBlur : public PostProcessingEffect, public CameraRequester {

	public:
		MotionBlur(float strength = 0.5f, unsigned int LOD = 15);
		
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		
		float getStrength() const;
		void  setStrength(float value);

		unsigned int getLevelOfDetail() const;
		void setLevelOfDetail(unsigned int value);

	protected:
		const FrameBufferInformation* screenInfo;

		MotionBlur(const std::string& shaderPath, float strength = 0.5f, unsigned int LOD = 15);
		virtual void bindValues();

	private:
		float strength;
		unsigned int LOD;
		glm::vec3 prevPosition;

		ShaderLocation samplesLocation;
		ShaderLocation strengthLocation;
		ShaderLocation offsetLocation;

	};


	//Motion blur that uses blurred image of previous frame to achieve the effect
	class MotionBlurGaussian : public MotionBlur {

	public:
		MotionBlurGaussian(GaussianBlur& blur, float strength = 0.5f, unsigned int LOD = 15);

		virtual void setBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void draw();

	private:
		GaussianBlur& blur;
		ColorBuffer prevFrame;

	};

}

#endif
