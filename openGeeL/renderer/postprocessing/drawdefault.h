#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "memory/memoryobject.h"
#include "postprocessing.h"

namespace geeL {

	class ImageTexture;
	class ITexture;

	class DefaultPostProcess : public PostProcessingEffectFS {

	public:
		DefaultPostProcess(float exposure = 1.f);

		DefaultPostProcess(const DefaultPostProcess& other);
		DefaultPostProcess& operator= (const DefaultPostProcess& other);


		virtual void init(const PostProcessingParameter& parameter);
		virtual void draw();

		void setCustomImage(const ITexture* const texture = nullptr);

		float getExposure() const;
		void setExposure(float exposure);

		virtual std::string toString() const;

	protected:
		del_unique_ptr<ImageTexture> noise;
		const ITexture* customTexture;

	};


	inline std::string DefaultPostProcess::toString() const {
		return "Default";
	}

}

#endif

