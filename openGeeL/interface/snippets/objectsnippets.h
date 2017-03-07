#ifndef OBJECTSNIPPETS_H
#define OBJECTSNIPPETS_H

#include "guisnippets.h"

namespace geeL {

	class SceneObject;
	class MeshRenderer;
	class Light;

	class SceneObjectSnippet : public GUISnippet {

	public:
		SceneObjectSnippet(SceneObject& obj);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		SceneObject& obj;

	};


	class MeshRendererSnippet : public SceneObjectSnippet {

	public:
		MeshRendererSnippet(MeshRenderer& mesh);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		MeshRenderer& mesh;

	};


	class LightSnippet : public SceneObjectSnippet {

	public:
		LightSnippet(Light& light);

		virtual void draw(GUIContext* context);
		virtual std::string toString() const;

	private:
		Light& light;

	};


}


#endif

