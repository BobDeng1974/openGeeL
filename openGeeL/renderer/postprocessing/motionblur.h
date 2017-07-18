#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

#include <glm.hpp>
#include <string>
#include "../utility/worldinformation.h"
#include "../framebuffer/colorbuffer.h"
#include "postprocessing.h"

namespace geeL {

	class Camera;
	class GaussianBlurBase;
	class RenderTexture;


	//Simple motion blur that blurs in (inverse) direction of camera movement
	class MotionBlur : public PostProcessingEffect, public CameraRequester {

	public:
		MotionBlur(float strength = 0.5f, unsigned int LOD = 15);
		
		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);
		
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
		~VelocityBuffer();

		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);
		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

	protected:
		virtual void bindValues();

	private:
		glm::vec3 prevPosition;
		PassthroughEffect prevPositionEffect;
		ColorBuffer positionBuffer;
		RenderTexture* positionTexture = nullptr;

	};


	class MotionBlurPerPixel : public MotionBlur {

	public:
		MotionBlurPerPixel(VelocityBuffer& velocity, float strength = 0.5f, unsigned int LOD = 15);
		~MotionBlurPerPixel();

		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

	protected:
		virtual void bindValues();

	private:
		VelocityBuffer& velocity;
		ColorBuffer velocityBuffer;
		RenderTexture* velocityTexture = nullptr;

	};

}

#endif
