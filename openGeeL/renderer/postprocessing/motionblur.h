#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

#include <glm.hpp>
#include <string>
#include "utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class Camera;
	class GaussianBlurBase;
	class RenderTarget;


	//Simple motion blur that blurs in (inverse) direction of camera movement
	class MotionBlur : public PostProcessingEffectFS, public CameraRequester {

	public:
		MotionBlur(float strength = 0.5f, unsigned int LOD = 15);
		
		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();

		float getStrength() const;
		void  setStrength(float value);

		unsigned int getLevelOfDetail() const;
		void setLevelOfDetail(unsigned int value);

		virtual std::string toString() const;

	protected:
		ShaderLocation samplesLocation;
		ShaderLocation strengthLocation;
		ShaderLocation offsetLocation;

		MotionBlur(const std::string& shaderPath, float strength = 0.5f, unsigned int LOD = 15);

	private:
		float strength;
		unsigned int LOD;
		glm::vec3 prevPosition;

	};


	class VelocityBuffer : public PostProcessingEffectFS, public CameraRequester {

	public:
		VelocityBuffer();
		virtual ~VelocityBuffer();

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();

	private:
		glm::vec3 prevPosition;
		PassthroughEffect prevPositionEffect;
		RenderTarget* positionTexture = nullptr;

	};


	class MotionBlurPerPixel : public MotionBlur {

	public:
		MotionBlurPerPixel(VelocityBuffer& velocity, float strength = 0.5f, unsigned int LOD = 15);
		virtual ~MotionBlurPerPixel();

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();

	protected:
		virtual void bindSubImages();

	private:
		VelocityBuffer& velocity;
		RenderTarget* velocityTexture = nullptr;

	};



	inline std::string MotionBlur::toString() const {
		return "Motion Blur";
	}

}

#endif
