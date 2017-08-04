#ifndef POSTRENDERER_H
#define POSTRENDERER_H

#include <list>

namespace geeL {

	class PostProcessingEffect;
	class SSAO;
	class WorldMapRequester;


	//Interface for drawing/rendering classes that can 
	//draw post processing effects to screen
	class PostEffectDrawer {

	public:
		virtual void addSSAO(SSAO& ssao) = 0;
		virtual void addEffect(PostProcessingEffect& effect) = 0;
		virtual void addEffect(PostProcessingEffect& effect, WorldMapRequester& requester) = 0;
		virtual void addEffect(PostProcessingEffect& effect, std::list<WorldMapRequester*> requester) = 0;

	};

}

#endif
