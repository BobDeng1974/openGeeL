#include <vector>
#include <iostream>
#include "postprocessing/postprocessing.h"
#include "renderer/deferredrenderer.h"
#include "texturing/textureprovider.h"
#include "texturing/rendertexture.h"
#include "utility/rendertime.h"
#include "guiwrapper.h"
#include "systeminformation.h"

namespace geeL {

	int modulo(int a, int b) {
		int result = a % b;
		return result >= 0 ? result : result + b;
	}


	SystemInformation::SystemInformation(RenderWindow& window, 
		DeferredRenderer& renderer,
		float x, float y, 
		float width, float height) 
			: GUIElement(window, x, y, width, height)
			, renderer(renderer) {
	
		TextureProvider& provider = renderer.getTextureProvider();
		Resolution resolution = provider.getRenderResolution();

		texture = &provider.requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGBA16,
			FilterMode::None, WrapMode::ClampEdge, AnisotropicFilter::None);
	
		propertyVisualizer = new PostProcessingEffectCS("shaders/postprocessing/propertyvisualize.com.glsl");
		propertyVisualizer->setResolution(resolution);
		propertyVisualizer->setTextureTarget(*texture);
		propertyVisualizer->addTextureSampler(provider.requestProperties(), "gProperties");
	}

	SystemInformation::~SystemInformation() {
		delete texture;
		delete propertyVisualizer;
	}


	void SystemInformation::draw(GUIContext* context) {

		if (nk_begin(context, "System Information", nk_rect(x, y, width, height),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {

			drawTime(context, "Render Time:");

			nk_layout_row_dynamic(context, 30, 1);
			int reset = nk_button_label(context, "Reset");
			nk_layout_row_dynamic(context, 30, 2);
			int back = -nk_button_label(context, "<");
			int forward = nk_button_label(context, ">");

			if (reset != 0) {
				renderer.setScreenImage();
				position = -1;
			}
			else if (back != 0 || forward != 0) {
				std::vector<const ITexture*> textures(std::move(getBuffers()));
				unsigned int size = unsigned int(textures.size()) + 3;

				if (size != 0) {
					position = modulo((position + back + forward), size);

					switch (position) {
						case 0:
						case 1:
							renderer.setScreenImage(textures[position]);
							break;

						default:
							renderer.setScreenImage(textures[2]);
							break;
					}
					
				}
			}

			//Draw property visualizer if one of the properties is currently selected
			if (position >= 2) {
				Shader& shader = propertyVisualizer->getShader();
				shader.bind<unsigned int>("mode", position - 2);

				propertyVisualizer->draw();
			}
		}

		nk_end(context);
	}

	void SystemInformation::drawTime(GUIContext* context, std::string name) {
		nk_layout_row_dynamic(context, 30, 2);
		nk_label(context, name.c_str(), NK_TEXT_RIGHT);

		std::string t = std::to_string(RenderTime::deltaTime()) + " ms";
		nk_label(context, t.c_str(), NK_TEXT_LEFT);
	}

	std::vector<const ITexture*> SystemInformation::getBuffers() {
		TextureProvider& provider = renderer.getTextureProvider();

		std::vector<const ITexture*> buffers;
		buffers.reserve(3);

		buffers.push_back(&provider.requestAlbedo());
		buffers.push_back(&provider.requestNormal());
		buffers.push_back(texture);

		return buffers;
	}

}