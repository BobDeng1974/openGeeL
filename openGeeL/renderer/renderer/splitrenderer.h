#ifndef SPLITRENDERER_H
#define SPLITRENDERER_H

#include <utility>
#include "renderer.h"

using namespace std;

namespace geeL {

	struct RenderViewport {
		float x;
		float y;
		float width;
		float height;

		RenderViewport(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
	};

	class SplitRenderer : public Renderer {

	public:
		SplitRenderer(RenderWindow& window, Input& inputManagerz, RenderContext& context);

		virtual void init();
		virtual void render();
		virtual void draw();
		virtual void handleInput();

		void addRenderer(Renderer* renderer, RenderViewport view);

	private:
		vector<pair<Renderer*, RenderViewport>> renderers;

	};
}

#endif

