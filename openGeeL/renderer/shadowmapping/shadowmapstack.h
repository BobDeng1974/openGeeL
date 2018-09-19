#ifndef SHADOWMAPSTACK_H
#define SHADOWMAPSTACK_H

#define SHADOWSTACK_SIZE 2
#define COOKIESTACK_SIZE 1

#include <functional>
#include <map>

namespace geeL {

	class ITexture;
	class ITextureProvider;
	class Light;
	class Shader;

	enum class LightMapType;


	//Stack that control shadow map memory on GPU
	class ShadowmapStack {

	public:
		ShadowmapStack(ITextureProvider& provider);

		void update();
		void lateUpdate();

		void bind(Shader& shader);
		void staticBind(Shader& shader);

		void add(Light& light);
		void remove(Light& light);

		void iterActiveShadowmaps(std::function<void(Light&)> function);

	private:
		struct StackElement {
			const ITexture* texture;
			Light* light;
			bool updated;

			StackElement() 
				: texture(nullptr), light(nullptr), updated(false) {}

			void update(Light& light, LightMapType type);
			void update(const ITexture* texture);

		};
		
		ITextureProvider& provider;
		std::map<Light*, LightMapType> lights;

		StackElement stack2D[SHADOWSTACK_SIZE];
		StackElement stackCube[SHADOWSTACK_SIZE];
		StackElement stackCookie[SHADOWSTACK_SIZE];


	};

}

#endif
