#ifndef RENDERTIME_H
#define RENDERTIME_H

#include <map>

namespace geeL {

	//Class that holds information about elapsed time inside the application
	class Time {

	public:
		static float deltaTime;

		static void update();

	private:
		static float lastFrame;

	};


	enum class RenderPass {
		Geometry,
		Shadow,
		SSAO,
		Lighting,
		PostProcessing,
		GUI
	};

	//Class that holds information about times of different rendering passes
	class RenderTime {

	public:
		RenderTime();

		void reset();
		void update(RenderPass pass);

		float getTime(RenderPass pass) const;

	private:
		std::map<RenderPass, float> passes;
		float lastTime;

	};
}

#endif

