#ifndef GUISNIPPETS_H
#define GUISNIPPETS_H

#include <string>
#include <vec3.hpp>
#include "../../renderer/materials/material.h"
#include "../../renderer/materials/material.h"
#include "../../renderer/materials/material.h"

struct nk_context;

namespace geeL {

	typedef nk_context GUIContext;

	class Transform;
	class Light;
	class GenericMaterialContainer;
	class DefaultMaterialContainer;


	class GUISnippet {

	public:
		virtual void draw(GUIContext* context) = 0;
		virtual std::string toString() const = 0;

	};

	class GUISnippets {

	public:

		static int drawBarInteger(GUIContext* context, int value, int min, int max, int step, std::string name);
		static float drawBarFloat(GUIContext* context, float value, float min, float max, float step, std::string name);

		static void drawVector(GUIContext* context, glm::vec3& vector, std::string prefix, float border, float step);
		static glm::vec3 drawVector2(GUIContext* context, glm::vec3& vector, std::string prefix, float border, float step);

		static void drawTransform(GUIContext* context, Transform& transform);
		static void drawTransformTree(GUIContext* context, Transform& transform);
		static void drawColor(GUIContext* context, glm::vec3& color);

		static void drawMaterial(GUIContext* context, GenericMaterialContainer* material);
		static void drawMaterial(GUIContext* context, DefaultMaterialContainer* material);

	};
}

#endif

