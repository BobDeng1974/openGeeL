#ifndef MOTIONBLUR_H
#define MOTIONBLUR_H

#include <glm.hpp>
#include <list>
#include <string>
#include "utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class Camera;
	class RenderTexture;


	//Simple motion blur that blurs in (inverse) direction of camera movement
	class MotionBlur : public PostProcessingEffectFS, public CameraRequester {

	public:
		const unsigned int maxLOD;

		MotionBlur(float strength = 0.5f, float sigma = 5.f, unsigned int LOD = 15,
			unsigned int maxLOD = 20);
		
		virtual void bindValues();

		float getStrength() const;
		float getSigma() const;
		unsigned int getLevelOfDetail() const;

		void setStrength(float value);
		void setSigma(float value);
		void setLevelOfDetail(unsigned int value);

		virtual std::string toString() const;

	protected:
		ShaderLocation samplesLocation;
		ShaderLocation strengthLocation;

		MotionBlur(const std::string& shaderPath, float strength = 0.5f, float sigma = 5.f, 
			unsigned int LOD = 15, unsigned int maxLOD = 20);

	private:
		float strength, sigma;
		unsigned int LOD;

		std::list<glm::vec3> positions;


		void updateKernel();

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
		RenderTexture* positionTexture = nullptr;

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
		RenderTexture* velocityTexture = nullptr;

	};



	inline std::string MotionBlur::toString() const {
		return "Motion Blur";
	}

}

#endif
