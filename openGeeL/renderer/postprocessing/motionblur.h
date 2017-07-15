#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

#include <glm.hpp>
#include <string>
#include "../utility/worldinformation.h"
#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	class Camera;
	class GaussianBlurBase;


	//Simple motion blur that blurs in (inverse) direction of camera movement
	class MotionBlur : public PostProcessingEffect, public CameraRequester {

	public:
		MotionBlur(float strength = 0.5f, unsigned int LOD = 15);
		
		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer);
		
		float getStrength() const;
		void  setStrength(float value);

		unsigned int getLevelOfDetail() const;
		void setLevelOfDetail(unsigned int value);

	protected:
		ShaderLocation samplesLocation;
		ShaderLocation strengthLocation;
		ShaderLocation offsetLocation;

		MotionBlur(const std::string& shaderPath, float strength = 0.5f, unsigned int LOD = 15);
		virtual void bindValues();

	private:
		float strength;
		unsigned int LOD;
		glm::vec3 prevPosition;

	};


	class VelocityBuffer : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		VelocityBuffer();

		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer);
		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		glm::vec3 prevPosition;
		PassthroughEffect prevPositionEffect;
		ColorBuffer positionBuffer;

	};


	class MotionBlurPerPixel : public MotionBlur {

	public:
		MotionBlurPerPixel(VelocityBuffer& velocity, float strength = 0.5f, unsigned int LOD = 15);

		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer);

	protected:
		virtual void bindValues();

	private:
		VelocityBuffer& velocity;
		ColorBuffer velocityBuffer;

	};

}

#endif
