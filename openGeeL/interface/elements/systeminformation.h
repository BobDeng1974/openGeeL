#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H

#include <string>
#include <vector>
#include "guielement.h"

namespace geeL {

	class DeferredRenderer;
	class ITExture;
	class PostProcessingEffectCS;
	class RenderTexture;


	class SystemInformation : public GUIElement {

	public:
		SystemInformation(RenderWindow& window, DeferredRenderer& renderer,
			float x = 0.1f, float y = 0.1f, float width = 0.2f, float height = 0.2f);
		~SystemInformation();

		virtual void draw(GUIContext* context);

	private:
		int position = -1;
		DeferredRenderer& renderer;
		RenderTexture* texture;
		PostProcessingEffectCS* propertyVisualizer;

		void drawTime(GUIContext* context, std::string name);
		std::vector<const ITexture*> getBuffers();

	};
}

#endif