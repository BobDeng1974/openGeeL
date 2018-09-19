#include "lights/light.h"
#include "lights/lightmaps.h"
#include "shader/shader.h"
#include "texturing/texture.h"
#include "texturing/textureprovider.h"
#include "shadowmap.h"
#include "shadowmapstack.h"

namespace geeL {

	ShadowmapStack::ShadowmapStack(ITextureProvider& provider)
		: provider(provider) {}

	
	void ShadowmapStack::update() {

		//TODO: Use better heuristic later. Currently just the first 
		//i lights for each map type are being used
		size_t i1 = 0, i2 = 0, i3 = 0; 
		for (auto it(lights.begin()); it != lights.end(); it++) {
			Light& light = *it->first;
			LightMapType types = it->second;
			
			if (i1 < SHADOWSTACK_SIZE && contains(types, LightMapType::Shadow2D)) {
				light.setMapIndex(i1 + 1, LightMapType::Shadow2D);
				stack2D[i1].update(light, LightMapType::Shadow2D);
				i1++;
			}

			if (i2 < SHADOWSTACK_SIZE && contains(types, LightMapType::ShadowCube)) {
				light.setMapIndex(i2 + 1, LightMapType::ShadowCube);
				stackCube[i2].update(light, LightMapType::ShadowCube);
				i2++;
			}

			if (i3 < COOKIESTACK_SIZE && contains(types, LightMapType::Cookie)) {
				light.setMapIndex(i3 + 1, LightMapType::Cookie);
				stackCookie[i3].update(light, LightMapType::Cookie);
				i3++;
			}
		}

		for (int j = i1; j < SHADOWSTACK_SIZE; j++)
			stack2D[j].update(&provider.requestDummy2D());

		for (int j = i2; j < SHADOWSTACK_SIZE; j++)
			stackCube[j].update(&provider.requestDummyCube());

		for (int j = i3; j < COOKIESTACK_SIZE; j++)
			stackCookie[j].update(&provider.requestDummy2D());

	}

	void ShadowmapStack::lateUpdate() {
		for (int i = 0; i < SHADOWSTACK_SIZE; i++) {
			stack2D[i].updated = false;
			stackCube[i].updated = false;
		}

		for (int i = 0; i < COOKIESTACK_SIZE; i++)
			stackCookie[i].updated = false;
	}

	void ShadowmapStack::bind(Shader& shader) {
		for (int i = 0; i < SHADOWSTACK_SIZE; i++) {
			StackElement& e1 = stack2D[i];
			StackElement& e2 = stackCube[i];

			if (e1.updated) shader.addMap(*e1.texture, "shadowMaps[" + std::to_string(i) + "]");
			if (e2.updated) shader.addMap(*e2.texture, "shadowCubes[" + std::to_string(i) + "]");
		}

		for (int i = 0; i < COOKIESTACK_SIZE; i++) {
			StackElement& e1 = stackCookie[i];
			if (e1.updated) shader.addMap(*e1.texture, "cookieMaps[" + std::to_string(i) + "]");
		}
	}

	void ShadowmapStack::staticBind(Shader& shader) {
		update();
		bind(shader);
	}

	float p = 0.f; //TODO: remove this later
	void ShadowmapStack::add(Light& light) {

		LightMapType types = LightMapType::None;
		LightMapContainer container = light.getMaps();
		container.iter([&](const ITexture& texture, LightMapType type) {
			switch(type) {
				case LightMapType::Shadow2D:
					types = concat(types, LightMapType::Shadow2D);
					break;
				case LightMapType::ShadowCube:
					types = concat(types, LightMapType::ShadowCube);
					break;
				case LightMapType::Cookie:
					types = concat(types, LightMapType::Cookie);
					break;
			}
		});

		if (types != LightMapType::None)
			lights[&light] = types;

		p += 1;
	}


	void ShadowmapStack::remove(Light& light) {
		auto it(lights.find(&light));
		if (it == lights.end()) return;

		lights.erase(it);

		LightMapType type = it->second;
		if (contains(type, LightMapType::Shadow2D)) 
			light.setMapIndex(0, LightMapType::Shadow2D);
		if (contains(type, LightMapType::ShadowCube)) 
			light.setMapIndex(0, LightMapType::ShadowCube);
		if (contains(type, LightMapType::Cookie))
			light.setMapIndex(0, LightMapType::Cookie);
	}

	void ShadowmapStack::iterActiveShadowmaps(std::function<void(Light&)> function) {
		for (int i = 0; i < SHADOWSTACK_SIZE; i++) {
			StackElement& e1 = stack2D[i];
			StackElement& e2 = stackCube[i];

			if (e1.light) function(*e1.light);
			if (e2.light) function(*e2.light);
		}
	}



	void ShadowmapStack::StackElement::update(Light& light, LightMapType type) {
		if (this->light != &light) {
			if(this->light) this->light->setMapIndex(0, type);

			this->light = &light;
			this->texture = light.getMap(type);

			updated = true;
		}
	}

	void ShadowmapStack::StackElement::update(const ITexture* texture) {
		if (this->texture != texture) {
			this->light = nullptr;
			this->texture = texture;

			updated = true;
		}
	}

}