#ifndef SHADERPROVIDER_H
#define SHADERPROVIDER_H


namespace geeL {

	class RenderScene;
	class WorldMapProvider;

	//Provider class that can bind and update scene information into requesting shaders
	class ShaderProvider {

	public:
		ShaderProvider(RenderScene& scene, WorldMapProvider& mapProvider);


	private:
		RenderScene& scene;
		WorldMapProvider& mapProvider;

	};

}

#endif
