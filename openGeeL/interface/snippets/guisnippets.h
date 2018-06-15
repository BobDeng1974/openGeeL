#ifndef GUISNIPPETS_H
#define GUISNIPPETS_H

#include <string>
#include <vec2.hpp>
#include <vec3.hpp>
#include <functional>
#include "materials/material.h"
#include "materials/material.h"
#include "materials/material.h"

struct nk_context;

namespace geeL {

	typedef nk_context GUIContext;

	class Transform;
	class Light;
	class GenericMaterialContainer;
	class DefaultMaterialContainer;

	enum class ResolutionPreset;


	class GUISnippet {

	public:
		virtual ~GUISnippet() {}

		virtual void draw(GUIContext* context) = 0;
		virtual std::string toString() const = 0;

	};

	class GUISnippets {

	public:

		static int drawBarInteger(GUIContext* context, int value, int min, int max, int step, std::string name);
		static int drawBarIntegerLogarithmic(GUIContext* context, int value, int min, int max, int step, std::string name);

		static float drawBarFloat(GUIContext* context, float value, float min, float max, float step, std::string name);
		static float drawBarFloatLogarithmic(GUIContext* context, float value, float min, float max, float step, std::string name);

		static glm::vec2 drawVector(GUIContext* context, const glm::vec2& vector, std::string prefix, float border, float step);
		static glm::vec3 drawVector(GUIContext* context, const glm::vec3& vector, std::string prefix, float border, float step);

		static void drawTransform(GUIContext* context, Transform& transform);
		static void drawTransformTree(GUIContext* context, Transform& transform);
		static void drawColor(GUIContext* context, glm::vec3& color);

		static void drawMaterial(GUIContext* context, GenericMaterialContainer* material);
		static void drawMaterial(GUIContext* context, DefaultMaterialContainer* material);

		static void drawTreeNode(GUIContext* context, const std::string& name, bool minimized, std::function<void(GUIContext*)> function);

		static ResolutionPreset drawResolution(GUIContext* context, ResolutionPreset current);

	};
}

#endif

