#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "texturing/imagetexture.h"
#include "framebuffer/colorbuffer.h"
#include "utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class BilateralFilter;
	class RenderTexture;

	//Screen Space Ambient Occlusion Post Effect
	class SSAO : public PostProcessingEffectFS, public WorldMapRequester, public CameraRequester {

	public:
		SSAO(PostProcessingEffectFS& blur, float radius = 5.f, 
			const RenderResolution& resolution = RenderResolution::HALFSCREEN);
		SSAO(const SSAO& other);
		virtual ~SSAO();

		SSAO& operator= (const SSAO& other);

		//Empty setter since base image isn't needed for Occlusion
		virtual void setBuffer(unsigned int buffer) {}

		//Tell SSAO in which texture it will get drawn into. The effect
		//will try to blend content if given texture has already been 
		//filled with occlusion data
		void setTargetTexture(const Texture& texture);

		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();
		virtual void draw();
		virtual void fill();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		float getRadius() const;
		void setRadius(float radius);

		const RenderResolution& getResolution() const;

		virtual std::string toString() const;

	private:
		float radius;
		unsigned int sampleCount = 32;
		RenderResolution scale;

		ImageTexture* noiseTexture;
		std::vector<glm::vec3> kernel;
		std::vector<glm::vec3> noise;
		PostProcessingEffectFS& blur;

		ShaderLocation projectionLocation;
		RenderTexture* ssaoTexture = nullptr;

		//These objects are used if SSAO needs to be blended into an existing occlusion texture

		bool blend = false;
		RenderTexture* blurTexture = nullptr;
		PostProcessingEffectFS* blendEffect = nullptr;
		
	};


	inline std::string SSAO::toString() const {
		return "SSAO";
	}
}

#endif