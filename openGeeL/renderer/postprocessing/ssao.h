#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "../texturing/imagetexture.h"
#include "../framebuffer/colorbuffer.h"
#include "../utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class BilateralFilter;
	class RenderTexture;

	//Screen Space Ambient Occlusion Post Effect
	class SSAO : public PostProcessingEffectFS, public WorldMapRequester, public CameraRequester {

	public:
		SSAO(PostProcessingEffectFS& blur, float radius = 5.f, ResolutionScale resolution = HALFSCREEN);
		SSAO(const SSAO& other);
		~SSAO();

		SSAO& operator= (const SSAO& other);

		//Empty setter since base image isn't needed for SSAO

		virtual void setBuffer(unsigned int buffer) {}
		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);
		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		float getRadius() const;
		void setRadius(float radius);

		const ResolutionScale& getResolution() const;

	protected:
		virtual void bindValues();

	private:
		float radius;
		unsigned int sampleCount = 32;
		ResolutionScale resolution;

		ImageTexture* noiseTexture;
		std::vector<glm::vec3> kernel;
		std::vector<glm::vec3> noise;
		PostProcessingEffectFS& blur;

		ShaderLocation projectionLocation;
		RenderTexture* tempTexture = nullptr;
		
	};
}

#endif