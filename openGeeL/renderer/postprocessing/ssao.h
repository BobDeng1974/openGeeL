#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class BilateralFilter;
	class Texture2D;
	class RenderTexture;

	//Screen Space Ambient Occlusion Post Effect
	class SSAO : public PostProcessingEffectFS, public CameraRequester {

	public:
		SSAO(PostProcessingEffectFS& blur, 
			float radius = 5.f, 
			const ResolutionPreset& resolution = ResolutionPreset::HALFSCREEN);
		virtual ~SSAO();


		//Empty setter since base image isn't needed for Occlusion
		virtual void setBuffer(unsigned int buffer) {}

		//Tell SSAO in which texture it will get drawn into. The effect
		//will try to blend content if given texture has already been 
		//filled with occlusion data
		void setTargetTexture(const RenderTexture& texture);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();
		virtual void fill();

		float getRadius() const;
		void setRadius(float radius);

		const ResolutionPreset& getResolution() const;

		virtual std::string toString() const;

	private:
		float radius;
		unsigned int sampleCount = 32;
		ResolutionPreset scale;

		Texture2D* noiseTexture;
		std::vector<glm::vec3> kernel;
	
		PostProcessingEffectFS& blur;

		ShaderLocation projectionLocation;
		RenderTexture* ssaoTexture = nullptr;

		//These objects are used if SSAO needs to be blended into an existing occlusion texture

		bool blend = false;
		RenderTexture* blurTexture = nullptr;
		PostProcessingEffectFS* blendEffect = nullptr;


		SSAO(const SSAO& other) = delete;
		SSAO& operator=(const SSAO& other) = delete;
		
	};


	inline std::string SSAO::toString() const {
		return "SSAO";
	}
}

#endif