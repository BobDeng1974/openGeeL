#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

#include <glm.hpp>
#include "../utility/worldinformation.h"
#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	class Camera;
	class GaussianBlur;

	class MotionBlur : public PostProcessingEffect, public CameraRequester {

	public:
		MotionBlur(GaussianBlur& blur, float strength = 0.5f);

		virtual void setBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void draw();

		float getStrength() const;
		void  setStrength(float value);

	protected:
		virtual void bindValues();

	private:
		float strength;
		ColorBuffer prevFrame;
		GaussianBlur& blur;
		glm::vec3 prevPosition;

		ShaderLocation samplesLocation;
		ShaderLocation strengthLocation;
		ShaderLocation offsetLocation;
		const FrameBufferInformation* screenInfo;

	};

}

#endif
