#ifndef GUISNIPPETS_H
#define GUISNIPPETS_H

#include <vec3.hpp>

struct nk_context;

namespace geeL {

	typedef nk_context GUIContext;

	class Transform;
	class Light;

	class GUISnippets {

	public:

		static void drawVector(GUIContext* context, glm::vec3& vector, std::string prefix, float border, float step);
		static glm::vec3 drawVector2(GUIContext* context, glm::vec3& vector, std::string prefix, float border, float step);

		static void drawTransform(GUIContext* context, Transform& transform);
		static void drawTransformTree(GUIContext* context, Transform& transform);
		static void drawColor(GUIContext* context, glm::vec3& color);
		static void drawLight(GUIContext* context, Light& light);

	};
}

#endif

